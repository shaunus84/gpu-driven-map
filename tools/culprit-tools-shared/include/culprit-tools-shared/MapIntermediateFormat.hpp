#pragma once

#include <cstdint>
#include <type_traits>

// Shared intermediate map format between tools.
// This file defines the on-disk contract only.
// No STL containers, no engine types, no tool-specific logic.

namespace culprit::formats {

// ------------------------------------------------------------
// Constants
// ------------------------------------------------------------

static constexpr uint32_t kMapMagic = 0x504D4150;  // 'PMAP'
static constexpr uint32_t kMapVersion = 4;
static constexpr uint32_t kInvalidIndex = 0xFFFFFFFF;

// ------------------------------------------------------------
// File header
// ------------------------------------------------------------
// All offsets are absolute file offsets (from file start)
// All counts are element counts, not byte sizes

struct MapFileHeader {
    uint32_t magic;    // kMapMagic
    uint32_t version;  // kMapVersion

    // Entities
    uint32_t entityCount;
    uint64_t entitiesOffset;  // MapEntityRecord[entityCount]
    uint32_t propertyRecordCount;
    uint64_t propertyRecordsOffset;

    // Geometry resources

    uint32_t meshCount;  // number of mesh records
    uint32_t materialSetCount;
    uint32_t materialRefCount;

    uint64_t meshesOffset;        // MapMeshRecord[meshCount]
    uint64_t verticesOffset;      // MapVertex[]
    uint64_t indicesOffset;       // uint32_t[]
    uint64_t materialSetsOffset;  // MapMaterialSetRecord[]
    uint64_t materialRefsOffset;  // MapMaterialRef[]

    // String table

    uint64_t stringTableOffset;
    uint64_t stringTableSize;
};

static_assert(std::is_trivially_copyable_v<MapFileHeader>);
static_assert(std::is_standard_layout_v<MapFileHeader>);

// ------------------------------------------------------------
// Vertex format (engine-agnostic)
// ------------------------------------------------------------

struct MapVertex {
    float px, py, pz;    // position
    float nx, ny, nz;    // normal
    float u, v;          // texcoord (texel or normalized, builder decides)
    float surfaceIndex;  // face id
};

static_assert(std::is_trivially_copyable_v<MapVertex>);
static_assert(std::is_standard_layout_v<MapVertex>);

// ------------------------------------------------------------
// Mesh record
// ------------------------------------------------------------
// Each mesh corresponds to a single surface (face group)

struct MapMeshRecord {
    uint32_t vertexCount;
    uint32_t indexCount;

    uint32_t vertexOffset;  // offset into MapVertex buffer
    uint32_t indexOffset;   // offset into uint32 index buffer

    uint32_t surfaceCount;
    uint32_t _pad;
};

static_assert(std::is_trivially_copyable_v<MapMeshRecord>);
static_assert(std::is_standard_layout_v<MapMeshRecord>);

struct MapEntityRecord {
    uint32_t classnameOffset;
    uint32_t classnameLength;

    uint32_t propertiesOffset;  // index into MapPropertyRecord[]
    uint32_t propertyCount;

    float localTransform[16];

    uint32_t meshIndex;         // kInvalidIndex if no brushes
    uint32_t materialSetIndex;  // kInvalidIndex if no brushes
};

static_assert(std::is_trivially_copyable_v<MapEntityRecord>);
static_assert(std::is_standard_layout_v<MapEntityRecord>);

struct MapPropertyRecord {
    uint32_t keyOffset;
    uint32_t keyLength;

    uint32_t valueOffset;
    uint32_t valueLength;
};

static_assert(std::is_trivially_copyable_v<MapPropertyRecord>);
static_assert(std::is_standard_layout_v<MapPropertyRecord>);

struct MapMaterialSetRecord {
    uint32_t materialRefOffset;
    uint32_t materialRefCount;
};

static_assert(std::is_trivially_copyable_v<MapMaterialSetRecord>);
static_assert(std::is_standard_layout_v<MapMaterialSetRecord>);

struct MapMaterialRef {
    uint32_t nameOffset;
    uint32_t nameLength;
};

static_assert(std::is_trivially_copyable_v<MapMaterialRef>);
static_assert(std::is_standard_layout_v<MapMaterialRef>);

}  // namespace culprit::formats
