#pragma once

#include <vector>

#include "parse/MapAuthoredTypes.h"
#include "pipeline/CompiledBrushEntity.h"

namespace culprit::tools {

// Builds mesh geometry from an Entity.
// - If worldToCanonical AND canonicalToWorld are provided, geometry (positions,
//   normals, plane IDs, and tex axes) are converted into canonical space.
// - If they are null, geometry is built in the authored space as-is.
//
// `scale` is applied to positions after any canonicalisation.
CompiledBrushEntity CompileBrushEntity(const Entity& entity, float scale);

CompiledBrushEntity CompileBrushEntityWorld(const Entity& entity, float scale = 1.0f);

}  // namespace culprit::tools
