#pragma once

#include <vector>

#include "MapGeometryTypes.h"
#include "parse/MapAuthoredTypes.h"

namespace culprit::tools {

// ============================================================================
// BrushPlanes
//
// Builds a canonical plane representation for each face of a brush.
//
// This stage:
// - Converts authored face points into plane equations
// - Preserves plane orientation as authored (assumes TrenchBroom outward
// winding)
// - Preserves texture axes and scale exactly as authored
//
// This stage does NOT:
// - Generate geometry
// - Merge planes
// - Perform CSG
//
// The resulting planes are intended for convex brush CSG.
// =============================================================================

std::vector<FacePlanes> BuildFacePlanes(const Brush& brush);

}  // namespace culprit::tools
