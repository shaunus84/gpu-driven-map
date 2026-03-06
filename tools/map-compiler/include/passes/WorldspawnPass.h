#pragma once

#include "pipeline/CompiledBrushEntity.h"

namespace culprit::tools {

//
// WorldspawnPass
//
// ==============================
// Detects and compiles worldspawn
// ==============================
//
// Worldspawn:
//   - Compiles brush geometry
//   - Emits a shared mesh
//   - Emits a material set
//   - Assigns render intent to the entity
//
// materialSetIndex semantics:
//   Base offset into the flattened material reference array.
//   finalMaterial = materialSetIndex + surfaceIndex.
//

struct WorldspawnPass {
    MapBuildContext& ctx;
    bool found = false;

    void Process(const Entity& e, uint32_t entityIndex) {
        if (!IsWorldspawn(e)) {
            return;
        }

        // ==============================
        // Enforce single worldspawn
        // ==============================

        if (found) {
            return;
        }

        found = true;

        // ==============================
        // Compile brush geometry
        // ==============================

        CompiledBrushEntity built = CompileBrushEntity(e, ctx.tbToEngineScale);

        // ==============================
        // Emit shared mesh
        // ==============================

        const uint32_t meshIndex = static_cast<uint32_t>(ctx.result.meshes.size());

        ctx.result.meshes.push_back(std::move(built.mesh));

        // ==============================
        // Emit material set
        // ==============================

        const uint32_t setIndex = static_cast<uint32_t>(ctx.result.materialSets.size());
        ctx.result.materialSets.push_back(std::move(built.materialSet));

        // ==============================
        // Assign render intent
        // ==============================

        EntityPrototype& entity = ctx.result.entities[entityIndex];

        entity.meshIndex = meshIndex;
        entity.materialSetIndex = setIndex;

        // ==============================
        // Set local transform (pivot)
        // ==============================

        entity.localTransform = maths::make_translation(built.pivot);
    }
};

}  // namespace culprit::tools
