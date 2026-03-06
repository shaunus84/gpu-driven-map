#pragma once

#include <filesystem>

#include "mesh/MeshTypes.h"
#include "parse/MapAuthoredTypes.h"

namespace culprit::tools {

struct MapBuildContext {
  explicit MapBuildContext(const std::filesystem::path& _workingDir, const std::vector<Entity>& _entities,
                           float scale)
      : workingDir{_workingDir}
      , entities{_entities}
      , tbToEngineScale{scale} {}

  // External
  const std::filesystem::path& workingDir;
  const std::vector<Entity>& entities;

  // Internal
  ParsedMap result{};
  float tbToEngineScale{0.0f};
  uint32_t materialCursor{0};
};

}  // namespace culprit::tools
