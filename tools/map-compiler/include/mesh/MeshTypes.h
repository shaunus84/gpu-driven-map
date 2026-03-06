#pragma once

#include <culprit-maths/Maths.hpp>
#include <culprit-tools-shared/MapIntermediateFormat.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace culprit::tools {

// ===========================================================
// Shared geometry (tool-side)
// ===========================================================

struct MeshPrototype {
  std::vector<formats::MapVertex> vertices;
  std::vector<uint32_t> indices;
  uint32_t surfaceCount{0};
};

// ===========================================================
// Materials (tool-side)
// ===========================================================

struct MaterialSet {
  std::vector<std::string> materials;
};

// ===========================================================
// Entity (tool-side)
// ===========================================================

struct EntityPrototype {
  std::string classname;
  std::unordered_map<std::string, std::string> properties;

  uint32_t meshIndex{formats::kInvalidIndex};
  uint32_t materialSetIndex{formats::kInvalidIndex};
  maths::Mat4f localTransform{maths::Mat4f::identity()};
};

// ===========================================================
// Parsed map (tool-side)
// ===========================================================
//
// This is the authoritative semantic representation produced
// by the map parser and consumed by later compilation stages.
//
// No runtime layout, no batching, no render instances.
// ===========================================================

struct ParsedMap {
  std::vector<EntityPrototype> entities;
  std::vector<MeshPrototype> meshes;
  std::vector<MaterialSet> materialSets;
};

}  // namespace culprit::tools
