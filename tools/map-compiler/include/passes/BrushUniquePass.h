#pragma once

#include "parse/MapAuthoredTypes.h"
#include "pipeline/CompiledBrushEntity.h"

namespace culprit::tools {

struct BrushUniquePass {
  MapBuildContext& ctx;

  void Process(const Entity& e, uint32_t entityIndex) {
    // ==============================
    // Skip non-unique brush cases
    // ==============================

    if (IsWorldspawn(e) || e.brushes.empty()) {
      return;
    }

    // Linked groups handled in BrushLinkedGroupPass
    if (IsFuncGroup(e)) {
      const std::string_view gid = GetLinkedGroupId(e);

      if (!gid.empty()) {
        return;
      }
    }

    // ==============================
    // Compile brush geometry
    // ==============================

    CompiledBrushEntity built = CompileBrushEntity(e, ctx.tbToEngineScale);

    const uint32_t meshIndex = static_cast<uint32_t>(ctx.result.meshes.size());

    ctx.result.meshes.push_back(std::move(built.mesh));

    const uint32_t setIndex = static_cast<uint32_t>(ctx.result.materialSets.size());
    ctx.result.materialSets.push_back(std::move(built.materialSet));

    ctx.materialCursor += ctx.result.meshes.back().surfaceCount;

    // ==============================
    // Assign render intent
    // ==============================

    EntityPrototype& proto = ctx.result.entities[entityIndex];

    proto.meshIndex = meshIndex;
    proto.materialSetIndex = setIndex;

    // Geometry is pivot-relative
    proto.localTransform = maths::make_translation(built.pivot);
  }
};

}  // namespace culprit::tools
