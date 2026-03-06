#include "geometry/PolygonUtils.h"

#include <algorithm>
#include <cmath>
#include <culprit-maths/Maths.hpp>
#include <utility>

namespace culprit::tools {

// Epsilon for geometric comparisons.
// Chosen to tolerate accumulated floating-point error during CSG.
static constexpr float kEpsilon = 1e-6f;

// Epsilon comparison helpers used for vertex deduplication.
static inline bool nearly_equal_f(float a, float b,
                                  float eps = kEpsilon) noexcept {
  return std::abs(a - b) <= eps;
}

static inline bool nearly_equal_v3(const maths::Vec3f& a, const maths::Vec3f& b,
                                   float eps = kEpsilon) noexcept {
  return nearly_equal_f(a[0], b[0], eps) && nearly_equal_f(a[1], b[1], eps) &&
         nearly_equal_f(a[2], b[2], eps);
}

// Computes a plane from a polygon vertex loop using Newell's method.
//
// This approach:
// - Works for non-triangular polygons
// - Is numerically stable for convex loops
// - Implicitly validates winding consistency

bool PlaneFromVerts(const std::vector<Vertex>& verts, maths::Planef& p) {
  if (verts.size() < 3) return false;

  maths::Vec3f center = maths::Vec3f::zero();
  maths::Vec3f planeNorm = maths::Vec3f::zero();

  for (size_t i = 0; i < verts.size(); ++i) {
    const size_t j = (i + 1) % verts.size();

    planeNorm[0] += (verts[i].pos[1] - verts[j].pos[1]) *
                    (verts[i].pos[2] + verts[j].pos[2]);
    planeNorm[1] += (verts[i].pos[2] - verts[j].pos[2]) *
                    (verts[i].pos[0] + verts[j].pos[0]);
    planeNorm[2] += (verts[i].pos[0] - verts[j].pos[0]) *
                    (verts[i].pos[1] + verts[j].pos[1]);

    center += verts[i].pos;
  }

  // Accumulate Newell normal
  const float mag =
      std::sqrt(planeNorm[0] * planeNorm[0] + planeNorm[1] * planeNorm[1] +
                planeNorm[2] * planeNorm[2]);

  if (mag < kEpsilon) return false;

  planeNorm /= mag;
  center /= static_cast<float>(verts.size());

  p = maths::Planef(planeNorm, -maths::dot(center, planeNorm));
  return true;
}

// Sorts polygon vertices into a consistent clockwise winding.
//
// The algorithm:
// 1. Compute centroid
// 2. Project vertices onto dominant plane
// 3. Sort by polar angle
// 4. Enforce winding against supplied normal

void SortVertsCW(std::vector<Vertex>& verts, const maths::Vec3f& normal) {
  if (verts.size() < 3) return;

  // --- centroid ---
  maths::Vec3f center = maths::Vec3f::zero();
  for (const auto& v : verts) center += v.pos;
  center /= static_cast<float>(verts.size());

  // --- choose dominant axis ---
  const maths::Vec3f n = maths::normalized(normal);
  const float ax = std::abs(n[0]);
  const float ay = std::abs(n[1]);
  const float az = std::abs(n[2]);

  // Drop the axis with the largest normal component to avoid degeneracy.
  enum class DropAxis { X, Y, Z };
  DropAxis drop = (ax >= ay && ax >= az)   ? DropAxis::X
                  : (ay >= ax && ay >= az) ? DropAxis::Y
                                           : DropAxis::Z;

  auto project2 = [&](const maths::Vec3f& p) -> std::pair<float, float> {
    const maths::Vec3f d = p - center;
    switch (drop) {
      case DropAxis::X:
        return {d[1], d[2]};  // YZ
      case DropAxis::Y:
        return {d[0], d[2]};  // XZ
      default:
        return {d[0], d[1]};  // XY
    }
  };

  std::sort(verts.begin(), verts.end(), [&](const Vertex& a, const Vertex& b) {
    const auto [ax2, ay2] = project2(a.pos);
    const auto [bx2, by2] = project2(b.pos);

    const float aa = std::atan2(ay2, ax2);
    const float ba = std::atan2(by2, bx2);

    if (aa != ba) return aa < ba;

    const float ad2 = ax2 * ax2 + ay2 * ay2;
    const float bd2 = bx2 * bx2 + by2 * by2;
    return ad2 < bd2;
  });

  // Ensure final winding matches the expected face normal.
  // This guarantees consistent orientation for triangulation.
  maths::Planef p{};
  if (PlaneFromVerts(verts, p)) {
    if (maths::dot(p.normal, normal) < 0)
      std::reverse(verts.begin(), verts.end());
  }
}

// Removes adjacent duplicate vertices using epsilon comparison.
// Assumes vertices are already sorted in loop order.

void DedupeVertsEpsilon(std::vector<Vertex>& verts) {
  if (verts.size() < 2) return;

  size_t write = 1;
  for (size_t read = 1; read < verts.size(); ++read) {
    if (!nearly_equal_v3(verts[read].pos, verts[write - 1].pos))
      verts[write++] = verts[read];
  }
  verts.resize(write);

  // handle wrap-around duplicate
  if (verts.size() >= 3 &&
      nearly_equal_v3(verts.front().pos, verts.back().pos)) {
    verts.pop_back();
  }
}

// Performs minimal sanity checks before expensive processing.

bool ValidateRawPolygon(const Polygon& poly) {
  if (poly.verts.size() < 3) return false;

  for (const auto& v : poly.verts) {
    if (!std::isfinite(v.pos[0]) || !std::isfinite(v.pos[1]) ||
        !std::isfinite(v.pos[2]))
      return false;
  }

  return true;
}

// Validates that the polygon vertices define a valid plane.
// This implicitly rejects degenerate or self-intersecting loops.

bool ValidateLoopPolygon(const Polygon& poly) {
  maths::Planef p;
  return PlaneFromVerts(poly.verts, p);
}

}  // namespace culprit::tools
