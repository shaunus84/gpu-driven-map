#include "geometry/UVUtils.h"

#include <cmath>
#include <cstdint>
#include <culprit-maths/Maths.hpp>
#include <limits>

namespace culprit::tools {

namespace {

// Your material compiler guarantees this today.
// When you support variable texture sizes, pass these in (per material)
// instead.
static constexpr float kTexWidth = 512.0f;
static constexpr float kTexHeight = 512.0f;

inline float Abs(float x) { return std::fabs(x); }
inline float Floor(float x) { return std::floor(x); }
inline float Ceil(float x) { return std::ceil(x); }

}  // namespace

// -----------------------------------------------------------------------------
// CalculateTexcoordsTexelSpace
//
// Faithful port of your old approach:
// 1) Project world-space vertex positions onto TrenchBroom/Valve texture axes.
// 2) Convert to normalized UV space by dividing by texture size.
// 3) Apply per-face scale (smaller => more tiling).
// 4) Optionally rebase UVs so the face is near (0,0) without changing tiling.
// -----------------------------------------------------------------------------
void CalculateTexcoordsTexelSpace(const maths::Planef& uAxis,
                                  const maths::Planef& vAxis, float uScale,
                                  float vScale, std::vector<Vertex>& verts,
                                  bool normalizeToNearZero) noexcept {
  if (verts.empty()) {
    return;
  }

  // Avoid divide-by-zero if authoring produces 0.
  if (uScale == 0.0f) uScale = 1.0f;
  if (vScale == 0.0f) vScale = 1.0f;

  // ------------------------------------------------------------
  // 1) World-space planar projection -> normalized UV space
  //
  // Matches old code:
  //   U = dot(uAxisNormal, pos) / texWidth  / uScale + uAxisDist / texWidth
  //   V = dot(vAxisNormal, pos) / texHeight / vScale + vAxisDist / texHeight
  // ------------------------------------------------------------
  for (auto& v : verts) {
    float U = maths::dot(uAxis.normal, v.pos);
    U = (U / kTexWidth) / uScale;
    U = U + (uAxis.distance / kTexWidth);

    float V = maths::dot(vAxis.normal, v.pos);
    V = (V / kTexHeight) / vScale;
    V = V + (vAxis.distance / kTexHeight);

    v.texCoord = maths::Vec2f{U, V};
  }

  if (!normalizeToNearZero) {
    return;
  }

  // ------------------------------------------------------------
  // 2) Decide whether to rebase U and/or V (same idea as old code)
  //
  // If ANY vertex already has coordinate in (-1, 1), we skip rebasing that
  // axis. (This prevents shifting faces that are already close to origin.)
  // ------------------------------------------------------------
  bool doU = true;
  bool doV = true;

  for (const auto& v : verts) {
    if (v.texCoord[0] < 1.0f && v.texCoord[0] > -1.0f) {
      doU = false;
    }
    // NOTE: your old code had a bug here (>1). This is the correct check.
    if (v.texCoord[1] < 1.0f && v.texCoord[1] > -1.0f) {
      doV = false;
    }
  }

  if (!doU && !doV) {
    return;
  }

  // ------------------------------------------------------------
  // 3) Find the integer tile boundary nearest to 0 (old behavior)
  // ------------------------------------------------------------
  float nearestU = 0.0f;
  float nearestV = 0.0f;

  if (doU) {
    const float u0 = verts[0].texCoord[0];
    nearestU = (u0 > 1.0f) ? Floor(u0) : Ceil(u0);
  }

  if (doV) {
    const float v0 = verts[0].texCoord[1];
    nearestV = (v0 > 1.0f) ? Floor(v0) : Ceil(v0);
  }

  for (const auto& v : verts) {
    if (doU) {
      const float U = v.texCoord[0];
      if (Abs(U) < Abs(nearestU)) {
        nearestU = (U > 1.0f) ? Floor(U) : Ceil(U);
      }
    }

    if (doV) {
      const float V = v.texCoord[1];
      if (Abs(V) < Abs(nearestV)) {
        nearestV = (V > 1.0f) ? Floor(V) : Ceil(V);
      }
    }
  }

  // ------------------------------------------------------------
  // 4) Rebase UVs (preserves tiling; removes large offsets)
  // ------------------------------------------------------------
  for (auto& v : verts) {
    if (doU) v.texCoord[0] -= nearestU;
    if (doV) v.texCoord[1] -= nearestV;
  }
}

}  // namespace culprit::tools
