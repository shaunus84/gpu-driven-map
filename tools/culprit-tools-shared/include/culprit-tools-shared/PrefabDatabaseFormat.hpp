#pragma once

#include <cstdint>

namespace culprit::formats {

// ============================================================
// Constants
// ============================================================

static constexpr uint32_t kPrefabMagic   = 0x50465242; // 'PFRB'
static constexpr uint32_t kPrefabVersion = 1;

// ============================================================
// File header
// ============================================================
// All offsets are absolute file offsets from the beginning of the file.
// All counts are element counts (not byte sizes).
// Strings are stored in a single UTF-8 string table at the end of the file.

struct PrefabFileHeader {
  uint32_t magic;    // kPrefabMagic
  uint32_t version;  // kPrefabVersion

  uint32_t prefabCount;
  uint32_t renderPrefabCount;
  uint32_t materialCount;
  uint32_t overrideCount;

  uint64_t prefabsOffset;        // PrefabRecord[prefabCount]
  uint64_t renderPrefabsOffset;  // RenderPrefabRecord[renderPrefabCount]
  uint64_t materialsOffset;      // RenderMaterialRef[materialCount]
  uint64_t overridesOffset;      // PropertyOverride[overrideCount]

  uint64_t stringTableOffset;    // char[stringTableSize]
};

// ============================================================
// Prefab record
// ============================================================
// Identifies a prefab and links it to optional render + overrides.

struct PrefabRecord {
  // Classname (string table relative)
  uint32_t classnameOffset;
  uint32_t classnameLength;

  // Optional render prefab
  // UINT32_MAX means "no render facet"
  uint32_t renderPrefabIndex;

  // Overrides range
  uint32_t overrideIndex; // index into PropertyOverride[]
  uint32_t overrideCount;

  uint32_t flags; // reserved for future use
};

// ============================================================
// Render prefab
// ============================================================
// Shared render data referenced by one or more prefabs.

struct RenderPrefabRecord {
  // Model path (string table relative)
  uint32_t modelPathOffset;
  uint32_t modelPathLength;

  // Materials range
  uint32_t materialIndex; // index into RenderMaterialRef[]
  uint32_t materialCount;

  // Optional bounds (authoring or computed)
  float boundsMin[3];
  float boundsMax[3];
};

struct RenderMaterialRef {
  // Material path (string table relative)
  uint32_t pathOffset;
  uint32_t pathLength;
};

// ============================================================
// Property override
// ============================================================
// Key/value string pairs, prefab-local.

struct PropertyOverride {
  uint32_t nameOffset;
  uint32_t nameLength;

  uint32_t valueOffset;
  uint32_t valueLength;
};

} // namespace culprit::formats
