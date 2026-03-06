#include "pipeline/MapParsePipeline.h"

#include <cstdint>
#include <iostream>
#include <vector>

#include "passes/BrushLinkedGroupPass.h"
#include "passes/BrushUniquePass.h"
#include "passes/PointEntityTransformPass.hpp"
#include "passes/WorldspawnPass.h"
#include "pipeline/MapBuildContext.h"

namespace culprit::tools {

ParsedMap ParseMapToIntermediate(MapBuildContext& context) {
  if (context.entities.empty()) {
    return ParsedMap{};
  }

  const size_t entityCount = context.entities.size();

  context.materialCursor = 0;
  context.result = {};

  // =====================================================
  // Initialise entity prototypes
  // =====================================================

  context.result.entities.resize(entityCount);

  for (size_t i = 0; i < entityCount; ++i) {
    const Entity& src = context.entities[i];
    EntityPrototype& dst = context.result.entities[i];

    dst.classname = src.classname;
    dst.properties = src.properties;

    dst.meshIndex = formats::kInvalidIndex;
    dst.materialSetIndex = formats::kInvalidIndex;
    dst.localTransform = maths::Mat4f::identity();
  }

  // Reserve shared resource storage.
  context.result.meshes.reserve(entityCount);
  context.result.materialSets.reserve(entityCount);

  // =====================================================
  // World Spawn
  // =====================================================
  {
    WorldspawnPass passWorld(context);
    for (size_t i = 0; i < entityCount; ++i) {
      passWorld.Process(context.entities[i], static_cast<uint32_t>(i));
    }
  }

  // =====================================================
  // Linked Group Instancing
  // =====================================================
  {
    BrushLinkedGroupPass passGroups(context);
    for (size_t i = 0; i < entityCount; ++i) {
      passGroups.Process(context.entities[i], static_cast<uint32_t>(i));
    }
    passGroups.Finalize();
  }

  // =====================================================
  // Unique Brush
  // =====================================================
  {
    BrushUniquePass passUnique(context);
    for (size_t i = 0; i < entityCount; ++i) {
      passUnique.Process(context.entities[i], static_cast<uint32_t>(i));
    }
  }

  // =====================================================
  // Point Entity Transform Pass
  // =====================================================
  {
    PointEntityTransformPass passPoint(context);
    for (size_t i = 0; i < entityCount; ++i) {
      passPoint.Process(context.entities[i], static_cast<uint32_t>(i));
    }
  }

  std::cout << "Entities: " << context.result.entities.size() << "\n";
  std::cout << "Meshes: " << context.result.meshes.size() << "\n";
  std::cout << "MaterialSets: " << context.result.materialSets.size() << "\n";

  return context.result;
}

}  // namespace culprit::tools
