#pragma once

#include <cstdint>
#include <culprit-shared/BinaryFileView.hpp>
#include <filesystem>
#include <string_view>
#include <iostream>

#include "MapIntermediateFormat.hpp"

namespace culprit::formats {

class MapIntermediateReader : public BinaryFileView {
 public:
  bool Open(const std::filesystem::path& path) {
    if (!LoadFile(path)) {
      return false;
    }

    if (size_ < sizeof(MapFileHeader)) {
      return false;
    }

    header_ = reinterpret_cast<const MapFileHeader*>(base_);

    if (header_->magic != kMapMagic || header_->version != kMapVersion) {
      return false;
    }

    if (!ValidateSections()) {
      return false;
    }

    return true;
  }

  void Close() {
    BinaryFileView::Close();
    header_ = nullptr;
  }

  bool IsValid() const {
    return header_ != nullptr;
  }

  const MapFileHeader& Header() const {
    return *header_;
  }

  // ===============================
  // Typed Accessors
  // ===============================

  const MapMeshRecord* Meshes() const {
    return TableAt<MapMeshRecord>(header_->meshesOffset, header_->meshCount);
  }

  const MapVertex* Vertices() const {
    return TableAt<MapVertex>(header_->verticesOffset, VertexCount());
  }

  const uint32_t* Indices() const {
    return TableAt<uint32_t>(header_->indicesOffset, IndexCount());
  }

  const MapEntityRecord* Entities() const {
    return TableAt<MapEntityRecord>(header_->entitiesOffset, header_->entityCount);
  }

  const MapPropertyRecord* PropertyRecords() const {
    return TableAt<MapPropertyRecord>(header_->propertyRecordsOffset, header_->propertyRecordCount);
  }

  const MapMaterialSetRecord* MaterialSets() const {
    return TableAt<MapMaterialSetRecord>(header_->materialSetsOffset, header_->materialSetCount);
  }

  const MapMaterialRef* MaterialRefs() const {
    return TableAt<MapMaterialRef>(header_->materialRefsOffset, header_->materialRefCount);
  }

  uint32_t EntityCount() const {
    return header_->entityCount;
  }

  uint32_t PropertyRecordCount() const {
    return header_->propertyRecordCount;
  }

  uint32_t VertexCount() const {
    uint32_t total = 0;
    const MapMeshRecord* meshes = Meshes();
    if (!meshes) {
      return 0;
    }

    for (uint32_t i = 0; i < header_->meshCount; ++i) {
      total += meshes[i].vertexCount;
    }

    return total;
  }

  uint32_t IndexCount() const {
    uint32_t total = 0;
    const MapMeshRecord* meshes = Meshes();
    if (!meshes) {
      return 0;
    }

    for (uint32_t i = 0; i < header_->meshCount; ++i) {
      total += meshes[i].indexCount;
    }

    return total;
  }

  std::string_view GetString(uint32_t offset, uint32_t length) const {
    if (!ValidateRange(header_->stringTableOffset + offset, length)) {
      return {};
    }

    return std::string_view(
        reinterpret_cast<const char*>(base_ + header_->stringTableOffset + offset), length);
  }

 private:
  bool ValidateSections() const {
    if (!ValidateRange(header_->entitiesOffset,
                       uint64_t(header_->entityCount) * sizeof(MapEntityRecord))) {
      return false;
    }

    if (!ValidateRange(header_->propertyRecordsOffset,
                       uint64_t(header_->propertyRecordCount) * sizeof(MapPropertyRecord))) {
      return false;
    }

    if (!ValidateRange(header_->meshesOffset,
                       uint64_t(header_->meshCount) * sizeof(MapMeshRecord))) {
      return false;
    }

    if (!ValidateRange(header_->materialSetsOffset,
                       uint64_t(header_->materialSetCount) * sizeof(MapMaterialSetRecord))) {
      return false;
    }

    if (!ValidateRange(header_->materialRefsOffset,
                       uint64_t(header_->materialRefCount) * sizeof(MapMaterialRef))) {
      return false;
    }

    if (!ValidateRange(header_->stringTableOffset, header_->stringTableSize)) {
      return false;
    }

    return true;
  }

 private:
  const MapFileHeader* header_ = nullptr;
};

}  // namespace culprit::formats
