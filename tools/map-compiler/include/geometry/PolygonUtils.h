#pragma once

#include <culprit-maths/Maths.hpp>
#include <vector>

#include "MapGeometryTypes.h"

namespace culprit::tools {

// =============================================================================
// PolygonUtils
//
// Utility functions for cleaning, validating, and ordering polygons produced
// by convex brush CSG.
//
// These functions operate on intermediate Polygon/Vertex data and are
// intentionally conservative:
// - They reject invalid geometry early
// - They do not attempt to "fix" broken input
// - They prioritise determinism over performance
// =============================================================================

// Computes a plane from a vertex loop using Newell's method.
// Returns false if the vertices are degenerate or collinear.
bool PlaneFromVerts(const std::vector<Vertex>& verts, maths::Planef& outPlane);

// Sorts vertices clockwise around the given normal.
// Produces a stable winding suitable for triangulation.
void SortVertsCW(std::vector<Vertex>& verts, const maths::Vec3f& normal);

// Removes duplicate vertices using an epsilon comparison.
// Assumes vertices are already approximately ordered.
void DedupeVertsEpsilon(std::vector<Vertex>& verts);

// Basic validation: checks vertex count and numeric sanity.
bool ValidateRawPolygon(const Polygon& poly);

// Loop validation: verifies the polygon defines a valid plane.
bool ValidateLoopPolygon(const Polygon& poly);

}  // namespace culprit::tools
