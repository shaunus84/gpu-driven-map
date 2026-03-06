#pragma once

#include <culprit-tools-shared/MapIntermediateFormat.hpp>
#include <string>
#include <vector>

#include "mesh/MeshTypes.h"
#include "parse/MapAuthoredTypes.h"
#include "pipeline/MapBuildContext.h"

namespace culprit::tools {

namespace {
constexpr std::string_view kWorldspawn = "worldspawn";
constexpr std::string_view kFuncGroup = "func_group";
constexpr std::string_view kTbLinkedGroupId = "_tb_linked_group_id";
constexpr std::string_view kTbTransformation = "_tb_transformation";
}  // namespace

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

inline const std::string* FindEntityProperty(const Entity& e, const std::string& key) {
  auto it = e.properties.find(key);
  return it == e.properties.end() ? nullptr : &it->second;
}

inline bool IsWorldspawn(const Entity& e) {
  return e.classname == kWorldspawn;
}

inline bool IsFuncGroup(const Entity& e) {
  return e.classname == kFuncGroup;
}

inline std::string_view GetLinkedGroupId(const Entity& e) {
  auto it = e.properties.find(std::string(kTbLinkedGroupId));
  return it == e.properties.end() ? std::string_view{} : std::string_view(it->second);
}

inline bool HasTbTransform(const Entity& e) {
  return e.properties.find(std::string(kTbTransformation)) != e.properties.end();
}

ParsedMap ParseMapToIntermediate(MapBuildContext& context);

}  // namespace culprit::tools
