#pragma once

#include "mesh/EntityMeshBuilder.h"
#include "mesh/MeshTypes.h"
#include "pipeline/CompiledBrushEntity.h"
#include "pipeline/MapBuildContext.h"

namespace culprit::tools {

namespace detail {
// ------------------------------------------------------------
// Minimal linear algebra (3D PCA-ish)
// ------------------------------------------------------------

struct AxisFrame {
  maths::Vec3f c{0.f, 0.f, 0.f};   // centroid (world)
  maths::Vec3f a0{1.f, 0.f, 0.f};  // principal axis 0 (unit)
  maths::Vec3f a1{0.f, 1.f, 0.f};  // principal axis 1 (unit)
  maths::Vec3f a2{0.f, 0.f, 1.f};  // principal axis 2 (unit), RH
  bool valid = false;
};

maths::Vec3f SafeNormalize(const maths::Vec3f& v) {
  const float len2 = maths::dot(v, v);
  if (len2 <= 1e-12f)
    return maths::Vec3f{0.f, 0.f, 0.f};
  return v / std::sqrt(len2);
}

// Power iteration for symmetric 3x3 (covariance)
// Returns largest-eigenvector approx. Assumes matrix is symmetric.
struct Mat3 {
  float m[3][3]{};
};

maths::Vec3f Mul(const Mat3& A, const maths::Vec3f& v) {
  return maths::Vec3f{A.m[0][0] * v.x() + A.m[0][1] * v.y() + A.m[0][2] * v.z(),
                      A.m[1][0] * v.x() + A.m[1][1] * v.y() + A.m[1][2] * v.z(),
                      A.m[2][0] * v.x() + A.m[2][1] * v.y() + A.m[2][2] * v.z()};
}

Mat3 Sub(const Mat3& A, const Mat3& B) {
  Mat3 C{};
  for (int r = 0; r < 3; ++r)
    for (int c = 0; c < 3; ++c)
      C.m[r][c] = A.m[r][c] - B.m[r][c];
  return C;
}

float Dot(const maths::Vec3f& a, const maths::Vec3f& b) {
  return maths::dot(a, b);
}

maths::Vec3f PowerIterLargestEigenvector(const Mat3& C) {
  maths::Vec3f v{1.f, 0.3f, 0.7f};
  v = SafeNormalize(v);
  for (int i = 0; i < 32; ++i) {
    maths::Vec3f w = Mul(C, v);
    const float n2 = Dot(w, w);
    if (n2 <= 1e-12f)
      break;
    v = w / std::sqrt(n2);
  }
  return v;
}

// Compute covariance of points (centered at c)
Mat3 Covariance(const std::vector<formats::MapVertex>& verts, const maths::Vec3f& c, int maxSamples = 2048) {
  Mat3 C{};
  const size_t n = verts.size();
  if (n == 0)
    return C;

  // sample stride for speed
  const size_t stride = (n > size_t(maxSamples)) ? (n / size_t(maxSamples)) : 1;

  double xx = 0, xy = 0, xz = 0, yy = 0, yz = 0, zz = 0;
  size_t count = 0;

  for (size_t i = 0; i < n; i += stride) {
    const auto& v = verts[i];
    const double x = double(v.px) - double(c.x());
    const double y = double(v.py) - double(c.y());
    const double z = double(v.pz) - double(c.z());
    xx += x * x;
    xy += x * y;
    xz += x * z;
    yy += y * y;
    yz += y * z;
    zz += z * z;
    ++count;
  }
  if (count == 0)
    return C;

  const double inv = 1.0 / double(count);
  C.m[0][0] = float(xx * inv);
  C.m[0][1] = float(xy * inv);
  C.m[0][2] = float(xz * inv);
  C.m[1][0] = float(xy * inv);
  C.m[1][1] = float(yy * inv);
  C.m[1][2] = float(yz * inv);
  C.m[2][0] = float(xz * inv);
  C.m[2][1] = float(yz * inv);
  C.m[2][2] = float(zz * inv);

  return C;
}

AxisFrame ComputeAxes(const CompiledBrushEntity& worldBuilt) {
  AxisFrame f{};
  f.c = worldBuilt.pivot;

  const auto& V = worldBuilt.mesh.vertices;
  if (V.size() < 8) {
    f.valid = false;
    return f;
  }

  // covariance
  Mat3 C = Covariance(V, f.c);

  // largest eigenvector
  maths::Vec3f e0 = PowerIterLargestEigenvector(C);
  e0 = SafeNormalize(e0);
  if (maths::dot(e0, e0) < 1e-6f) {
    f.valid = false;
    return f;
  }

  // deflate to get second: C2 = C - lambda0 * outer(e0)
  const maths::Vec3f Ce0 = Mul(C, e0);
  const float lambda0 = maths::dot(e0, Ce0);
  Mat3 C2 = Sub(C, Mat3{{{lambda0 * e0.x() * e0.x(), lambda0 * e0.x() * e0.y(), lambda0 * e0.x() * e0.z()},
                         {lambda0 * e0.y() * e0.x(), lambda0 * e0.y() * e0.y(), lambda0 * e0.y() * e0.z()},
                         {lambda0 * e0.z() * e0.x(), lambda0 * e0.z() * e0.y(), lambda0 * e0.z() * e0.z()}}});

  maths::Vec3f e1 = PowerIterLargestEigenvector(C2);

  // orthogonalize e1 vs e0
  e1 = e1 - e0 * maths::dot(e1, e0);
  e1 = SafeNormalize(e1);

  // if degenerate, pick any perpendicular basis
  if (maths::dot(e1, e1) < 1e-6f) {
    // choose an arbitrary vector not parallel to e0
    maths::Vec3f t = (std::fabs(e0.z()) < 0.9f) ? maths::Vec3f{0.f, 0.f, 1.f} : maths::Vec3f{0.f, 1.f, 0.f};
    e1 = SafeNormalize(maths::cross(t, e0));
    if (maths::dot(e1, e1) < 1e-6f) {
      f.valid = false;
      return f;
    }
  }

  maths::Vec3f e2 = SafeNormalize(maths::cross(e0, e1));
  // re-orthogonalize e1 to ensure orthonormal
  e1 = SafeNormalize(maths::cross(e2, e0));

  f.a0 = e0;
  f.a1 = e1;
  f.a2 = e2;
  f.valid = true;
  return f;
}

// Compute extents along axis basis (min/max projection)
struct Extents1D {
  float mn = 0, mx = 0;
};

Extents1D ProjectExtents(const std::vector<formats::MapVertex>& V, const maths::Vec3f& c,
                         const maths::Vec3f& axis, int maxSamples = 2048) {
  Extents1D e{};
  e.mn = +std::numeric_limits<float>::infinity();
  e.mx = -std::numeric_limits<float>::infinity();

  const size_t n = V.size();
  if (n == 0) {
    e.mn = e.mx = 0.f;
    return e;
  }
  const size_t stride = (n > size_t(maxSamples)) ? (n / size_t(maxSamples)) : 1;

  for (size_t i = 0; i < n; i += stride) {
    const auto& v = V[i];
    maths::Vec3f p{v.px - c.x(), v.py - c.y(), v.pz - c.z()};
    const float t = maths::dot(p, axis);
    e.mn = std::min(e.mn, t);
    e.mx = std::max(e.mx, t);
  }
  if (!std::isfinite(e.mn) || !std::isfinite(e.mx)) {
    e.mn = e.mx = 0.f;
  }
  return e;
}

// Decide sign flips for axes to best match extents (heuristic,
// correspondence-free)
void FixAxisSignsByExtents(const std::vector<formats::MapVertex>& VA, const AxisFrame& A,
                           const std::vector<formats::MapVertex>& VB, AxisFrame& B) {
  // We align A.a0->B.a0, A.a1->B.a1, A.a2->B.a2 (already sorted by variance).
  // Choose sign for B axes to match projection extents.
  const Extents1D a0 = ProjectExtents(VA, A.c, A.a0);
  const Extents1D a1 = ProjectExtents(VA, A.c, A.a1);

  const Extents1D b0 = ProjectExtents(VB, B.c, B.a0);
  const Extents1D b1 = ProjectExtents(VB, B.c, B.a1);

  auto chooseSign = [](const Extents1D& a, const Extents1D& b) -> float {
    // if we flip axis, projections negate: [mn,mx] -> [-mx,-mn]
    const float costSame = std::fabs(b.mn - a.mn) + std::fabs(b.mx - a.mx);
    const float costFlip = std::fabs(b.mn - (-a.mx)) + std::fabs(b.mx - (-a.mn));
    return (costFlip < costSame) ? -1.f : +1.f;
  };

  const float s0 = chooseSign(a0, b0);
  const float s1 = chooseSign(a1, b1);

  B.a0 = B.a0 * s0;
  B.a1 = B.a1 * s1;

  // Recompute a2 to enforce RH and orthonormality
  B.a2 = SafeNormalize(maths::cross(B.a0, B.a1));
  B.a1 = SafeNormalize(maths::cross(B.a2, B.a0));
}

// R = [B] * [A]^T
maths::Mat4f AlignRotation(const AxisFrame& A, const AxisFrame& B) {
  // A basis columns
  const maths::Vec3f Ax = A.a0, Ay = A.a1, Az = A.a2;
  // B basis columns
  const maths::Vec3f Bx = B.a0, By = B.a1, Bz = B.a2;

  // Compute R3 = B * A^T (3x3)
  maths::Mat4f R = maths::Mat4f::identity();

  // column 0 of R = B * (row 0 of A)^T = B * [Ax.x Ay.x Az.x]^T
  R.at(0, 0) = Bx.x() * Ax.x() + By.x() * Ay.x() + Bz.x() * Az.x();
  R.at(1, 0) = Bx.y() * Ax.x() + By.y() * Ay.x() + Bz.y() * Az.x();
  R.at(2, 0) = Bx.z() * Ax.x() + By.z() * Ay.x() + Bz.z() * Az.x();

  // column 1
  R.at(0, 1) = Bx.x() * Ax.y() + By.x() * Ay.y() + Bz.x() * Az.y();
  R.at(1, 1) = Bx.y() * Ax.y() + By.y() * Ay.y() + Bz.y() * Az.y();
  R.at(2, 1) = Bx.z() * Ax.y() + By.z() * Ay.y() + Bz.z() * Az.y();

  // column 2
  R.at(0, 2) = Bx.x() * Ax.z() + By.x() * Ay.z() + Bz.x() * Az.z();
  R.at(1, 2) = Bx.y() * Ax.z() + By.y() * Ay.z() + Bz.y() * Az.z();
  R.at(2, 2) = Bx.z() * Ax.z() + By.z() * Ay.z() + Bz.z() * Az.z();

  return R;
}
}  // namespace detail

struct BrushLinkedGroupPass {
  MapBuildContext& ctx;

  std::unordered_map<std::string, std::vector<uint32_t>> groupMembers;

  BrushLinkedGroupPass(MapBuildContext& c)
      : ctx(c) {
    groupMembers.reserve(ctx.entities.size());
  }

  void Process(const Entity& e, uint32_t entityIndex) {
    if (IsWorldspawn(e) || e.brushes.empty())
      return;

    if (!IsFuncGroup(e))
      return;

    const std::string_view gid = GetLinkedGroupId(e);
    if (gid.empty())
      return;

    groupMembers[std::string{gid}].push_back(entityIndex);
  }

  void Finalize() {
    for (auto& [gid, members] : groupMembers) {
      const uint32_t canonicalIdx = SelectCanonical(members);

      const Entity& canonical = ctx.entities[canonicalIdx];

      // === Build canonical geometry ===

      CompiledBrushEntity canonLocal = CompileBrushEntity(canonical, ctx.tbToEngineScale);

      CompiledBrushEntity canonWorld = CompileBrushEntityWorld(canonical, ctx.tbToEngineScale);

      const uint32_t meshIndex = static_cast<uint32_t>(ctx.result.meshes.size());
      ctx.result.meshes.push_back(std::move(canonLocal.mesh));

      const uint32_t setIndex = static_cast<uint32_t>(ctx.result.materialSets.size());
      ctx.result.materialSets.push_back(std::move(canonLocal.materialSet));

      detail::AxisFrame frameA = detail::ComputeAxes(canonWorld);

      const auto& canonWorldVerts = canonWorld.mesh.vertices;

      // === Emit group members ===

      for (uint32_t idx : members) {
        EntityPrototype& proto = ctx.result.entities[idx];

        maths::Mat4f R = maths::Mat4f::identity();

        maths::Vec3f pivot;

        if (idx == canonicalIdx) {
          pivot = canonWorld.pivot;
        } else {
          CompiledBrushEntity sibWorld = CompileBrushEntityWorld(ctx.entities[idx], ctx.tbToEngineScale);

          pivot = sibWorld.pivot;

          detail::AxisFrame frameB = detail::ComputeAxes(sibWorld);

          if (frameA.valid && frameB.valid) {
            detail::FixAxisSignsByExtents(canonWorldVerts, frameA, sibWorld.mesh.vertices, frameB);

            R = detail::AlignRotation(frameA, frameB);
          }
        }

        proto.meshIndex = meshIndex;
        proto.materialSetIndex = setIndex;
        proto.localTransform = maths::make_translation(pivot) * R;
      }
    }
  }

 private:
  uint32_t SelectCanonical(const std::vector<uint32_t>& members) const {
    for (uint32_t idx : members) {
      if (!HasTbTransform(ctx.entities[idx]))
        return idx;
    }
    return members.front();
  }
};

}  // namespace culprit::tools
