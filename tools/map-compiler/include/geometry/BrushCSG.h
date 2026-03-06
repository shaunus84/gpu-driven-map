#pragma once

#include <vector>

#include "MapGeometryTypes.h"

namespace culprit::tools {

// =============================================================================
// BrushCSG
//
// Generates raw polygon vertex sets for a convex brush by intersecting
// all triplets of face planes.
//
// IMPORTANT:
// - Output polygons contain UNORDERED vertices.
// - Vertices form a point cloud lying on each face plane.
// - No winding, sorting, triangulation, or deduplication is performed here.
// - One Polygon is produced per face plane.
//
// This stage exists purely to extract geometric intersections.
// Higher-level processing (ordering, UVs, materials) happens later.
// =============================================================================

std::vector<Polygon> BuildBrushPolygons(const std::vector<FacePlanes>& planes);

}  // namespace culprit::tools
