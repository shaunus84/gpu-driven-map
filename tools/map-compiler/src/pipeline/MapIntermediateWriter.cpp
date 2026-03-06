#include "pipeline/MapIntermediateWriter.h"

// MapIntermediateWriter
//
// Converts tool-side map into a flat, POD-only intermediate format.

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <culprit-tools-shared/MapIntermediateFormat.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace culprit::tools {

namespace {

template <typename T>
bool WritePOD(std::ofstream& out, const T& v) {
  static_assert(std::is_trivially_copyable_v<T>);
  out.write(reinterpret_cast<const char*>(&v), sizeof(T));
  return static_cast<bool>(out);
}

template <typename T>
bool WriteArray(std::ofstream& out, const T* data, size_t count) {
  static_assert(std::is_trivially_copyable_v<T>);
  if (count == 0)
    return true;
  out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(sizeof(T) * count));
  return static_cast<bool>(out);
}

uint64_t Tell(std::ofstream& out) {
  return static_cast<uint64_t>(out.tellp());
}

}  // namespace

bool WriteIntermediateMap(const std::string& outputPath, const ParsedMap& map) {
  using namespace culprit::formats;

  std::ofstream out(outputPath, std::ios::binary);
  if (!out) {
    return false;
  }

  // ============================================================
  // Collect all material strings (flat, then sort+unique)
  // ============================================================

  std::vector<std::string> strings;

  // Materials
  for (const auto& ms : map.materialSets) {
    for (const std::string& mat : ms.materials) {
      strings.push_back(mat);
    }
  }

  // Entities
  for (const auto& e : map.entities) {
    strings.push_back(e.classname);

    for (const auto& [k, v] : e.properties) {
      strings.push_back(k);
      strings.push_back(v);
    }
  }

  std::sort(strings.begin(), strings.end());
  strings.erase(std::unique(strings.begin(), strings.end()), strings.end());

  auto findStringIndex = [&](const std::string& s) -> uint32_t {
    auto it = std::lower_bound(strings.begin(), strings.end(), s);
    return static_cast<uint32_t>(it - strings.begin());
  };

  // ============================================================
  // Build packed string table
  // ============================================================

  std::vector<uint32_t> stringOffsets(strings.size());
  uint32_t totalStringBytes = 0;

  for (size_t i = 0; i < strings.size(); ++i) {
    stringOffsets[i] = totalStringBytes;
    totalStringBytes += static_cast<uint32_t>(strings[i].size() + 1);
  }

  std::vector<char> stringTable;
  stringTable.resize(totalStringBytes);

  {
    uint32_t cursor = 0;
    for (const auto& s : strings) {
      std::memcpy(stringTable.data() + cursor, s.data(), s.size());
      stringTable[cursor + s.size()] = '\0';
      cursor += static_cast<uint32_t>(s.size() + 1);
    }
  }

  // ============================================================
  // Pack mesh vertex and index buffers
  // ============================================================

  std::vector<MapMeshRecord> meshRecords;
  meshRecords.resize(map.meshes.size());

  size_t totalVerts = 0;
  size_t totalIndices = 0;

  for (const auto& mesh : map.meshes) {
    totalVerts += mesh.vertices.size();
    totalIndices += mesh.indices.size();
  }

  std::vector<MapVertex> outVertices;
  std::vector<uint32_t> outIndices;
  outVertices.reserve(totalVerts);
  outIndices.reserve(totalIndices);

  for (size_t i = 0; i < map.meshes.size(); ++i) {
    const MeshPrototype& mesh = map.meshes[i];

    MapMeshRecord& rec = meshRecords[i];
    rec.vertexOffset = static_cast<uint32_t>(outVertices.size());
    rec.indexOffset = static_cast<uint32_t>(outIndices.size());
    rec.vertexCount = static_cast<uint32_t>(mesh.vertices.size());
    rec.indexCount = static_cast<uint32_t>(mesh.indices.size());
    rec.surfaceCount = mesh.surfaceCount;

    outVertices.insert(outVertices.end(), mesh.vertices.begin(), mesh.vertices.end());
    outIndices.insert(outIndices.end(), mesh.indices.begin(), mesh.indices.end());
  }

  // ============================================================
  // Material sets and references
  // ============================================================

  std::vector<MapMaterialSetRecord> materialSetRecords;
  std::vector<MapMaterialRef> materialRefs;

  materialSetRecords.resize(map.materialSets.size());

  uint32_t materialRefCursor = 0;

  for (size_t i = 0; i < map.materialSets.size(); ++i) {
    const auto& ms = map.materialSets[i];

    MapMaterialSetRecord& rec = materialSetRecords[i];
    rec.materialRefOffset = materialRefCursor;
    rec.materialRefCount = static_cast<uint32_t>(ms.materials.size());

    for (const std::string& mat : ms.materials) {
      const uint32_t strIdx = findStringIndex(mat);

      MapMaterialRef ref{};
      ref.nameOffset = stringOffsets[strIdx];
      ref.nameLength = static_cast<uint32_t>(strings[strIdx].size());

      materialRefs.push_back(ref);
      ++materialRefCursor;
    }
  }

  // ============================================================
  // Pack property records
  // ============================================================

  std::vector<MapPropertyRecord> propertyRecords;
  propertyRecords.reserve(map.entities.size() * 4);

  // ============================================================
  // Pack entity records
  // ============================================================

  std::vector<MapEntityRecord> entityRecords;
  entityRecords.resize(map.entities.size());

  for (size_t i = 0; i < map.entities.size(); ++i) {
    const EntityPrototype& src = map.entities[i];
    MapEntityRecord& dst = entityRecords[i];

    // =========================================
    // Classname
    // =========================================

    uint32_t classStr = findStringIndex(src.classname);
    dst.classnameOffset = stringOffsets[classStr];
    dst.classnameLength = uint32_t(src.classname.size());

    // =========================================
    // Properties (sorted for determinism)
    // =========================================

    dst.propertiesOffset = uint32_t(propertyRecords.size());
    dst.propertyCount = uint32_t(src.properties.size());

    std::vector<std::pair<std::string, std::string>> sortedProps(src.properties.begin(),
                                                                 src.properties.end());

    std::sort(sortedProps.begin(), sortedProps.end(), [](const auto& a, const auto& b) {
      return a.first < b.first;
    });

    for (const auto& [key, value] : sortedProps) {
      uint32_t keyStr = findStringIndex(key);
      uint32_t valStr = findStringIndex(value);

      MapPropertyRecord rec{};
      rec.keyOffset = stringOffsets[keyStr];
      rec.keyLength = uint32_t(key.size());

      rec.valueOffset = stringOffsets[valStr];
      rec.valueLength = uint32_t(value.size());

      propertyRecords.push_back(rec);
    }

    // =========================================
    // Transform
    // =========================================

    std::memcpy(dst.localTransform, src.localTransform.data(), sizeof(float) * 16);

    // =========================================
    // Render intent
    // =========================================

    dst.meshIndex = src.meshIndex;
    dst.materialSetIndex = src.materialSetIndex;
  }

  for (size_t i = 0; i < map.materialSets.size(); ++i) {
    const auto& ms = map.materialSets[i];

    for (size_t j = 0; j < ms.materials.size(); ++j) {
      if (ms.materials[j].empty()) {
        std::cout << "[intermediate] EMPTY material in set " << i << " slot " << j << "\n";
      }
    }
  }

  // ============================================================
  // Write file
  // ============================================================

  MapFileHeader header{};
  header.magic = kMapMagic;
  header.version = kMapVersion;

  header.entityCount = uint32_t(entityRecords.size());
  header.meshCount = uint32_t(meshRecords.size());
  header.materialSetCount = uint32_t(materialSetRecords.size());
  header.materialRefCount = uint32_t(materialRefs.size());

  header.propertyRecordCount = uint32_t(propertyRecords.size());

  const uint64_t headerPos = Tell(out);
  WritePOD(out, header);

  // ============================================================
  // Entities
  // ============================================================

  header.entitiesOffset = Tell(out);
  WriteArray(out, entityRecords.data(), entityRecords.size());

  // ============================================================
  // Properties
  // ============================================================

  header.propertyRecordsOffset = Tell(out);
  WriteArray(out, propertyRecords.data(), propertyRecords.size());

  // ============================================================
  // Meshes
  // ============================================================

  header.meshesOffset = Tell(out);
  WriteArray(out, meshRecords.data(), meshRecords.size());

  header.verticesOffset = Tell(out);
  WriteArray(out, outVertices.data(), outVertices.size());

  header.indicesOffset = Tell(out);
  WriteArray(out, outIndices.data(), outIndices.size());

  // ============================================================
  // Materials
  // ============================================================

  header.materialSetsOffset = Tell(out);
  WriteArray(out, materialSetRecords.data(), materialSetRecords.size());

  header.materialRefsOffset = Tell(out);
  WriteArray(out, materialRefs.data(), materialRefs.size());

  // ============================================================
  // String table
  // ============================================================

  header.stringTableOffset = Tell(out);
  header.stringTableSize = uint64_t(stringTable.size());

  if (!stringTable.empty())
    out.write(stringTable.data(), std::streamsize(stringTable.size()));

  // Patch header
  out.seekp(std::streamoff(headerPos), std::ios::beg);
  WritePOD(out, header);

  return true;
}

}  // namespace culprit::tools
