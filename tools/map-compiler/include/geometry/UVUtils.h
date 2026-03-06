#pragma once

#include <culprit-maths/Maths.hpp>
#include <vector>

#include "MapGeometryTypes.h"

namespace culprit::tools {

// =============================================================================
// UVUtils
//
// Computes texture coordinates for brush polygons using Valve-style
// texture projection (U/V axes in plane form).
//
// This operates in *texel space*, not normalized [0,1] UVs.
// Normalization or wrapping is a renderer/runtime concern.
// =============================================================================

// Computes texel-space UVs for a polygon using texture projection axes.
//
// - uAxis / vAxis are plane equations defining texture projection
// - uScale / vScale are texture scale factors
// - verts are modified in place
// - Optional normalization shifts UVs closer to zero to improve stability

void CalculateTexcoordsTexelSpace(const maths::Planef& uAxis,
                                  const maths::Planef& vAxis, float uScale,
                                  float vScale, std::vector<Vertex>& verts,
                                  bool normalizeToNearZero = true) noexcept;

}  // namespace culprit::tools
