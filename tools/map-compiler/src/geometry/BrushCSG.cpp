#include "geometry/BrushCSG.h"

// This file implements classic convex brush CSG using plane intersections.
// The algorithm:
//   1. Intersect every triplet of planes.
//   2. Keep intersection points that lie inside all planes.
//   3. Assign each point to the faces it lies on.
//
// This is O(n^4) in the worst case, but n is small (brush faces).
// Correctness and determinism are prioritised over performance.

#include <cmath>
#include <culprit-maths/Maths.hpp>

namespace culprit::tools {

static constexpr float kEpsilon = 1e-6f;
static constexpr float kInsideEps = 1e-4f;
static constexpr float kOnPlaneEps = 1e-3f;  // face membership tolerance


// Computes the intersection point of three planes.
//
// Returns false if the planes do not intersect at a single point
// (i.e. they are parallel or nearly parallel).
//
// Maths note:
// The determinant test checks whether the three plane normals form
// a valid 3D basis (non-zero volume).

bool IntersectThreePlanes(const maths::Planef& p1,
                          const maths::Planef& p2,
                          const maths::Planef& p3,
                          maths::Vec3f& outPoint)
{
  const maths::Vec3f n1xn2 = maths::cross(p1.normal, p2.normal);
  const float det = maths::dot(n1xn2, p3.normal);

  if (std::abs(det) <= kEpsilon)
    return false;

  outPoint =
      (-p1.distance * maths::cross(p2.normal, p3.normal)
       -p2.distance * maths::cross(p3.normal, p1.normal)
       -p3.distance * n1xn2) / det;

  return true;
}

// Builds one raw Polygon per face plane by intersecting all plane triplets.
//
// The resulting polygons:
// - Contain unordered vertices
// - May contain duplicate points
// - Are NOT guaranteed to be valid loops
//
// This function assumes the input planes define a closed convex volume.

std::vector<Polygon> BuildBrushPolygons(const std::vector<FacePlanes>& planes) {
  // Initialise one output polygon per face.
  // Each polygon collects all vertices that lie on that face plane.
  std::vector<Polygon> polys;
  polys.resize(planes.size());

  // Initialise normals per face
  for (size_t i = 0; i < planes.size(); ++i) {
    polys[i].normal = planes[i].plane.normal;
    polys[i].faceIndex = i;
  }

  const size_t n = planes.size();

  // Intersect every combination of three planes.
  // Each valid intersection produces one candidate vertex.
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = i + 1; j < n; ++j) {
      for (size_t k = j + 1; k < n; ++k) {
        maths::Vec3f p;
        if (!IntersectThreePlanes(planes[i].plane, planes[j].plane,
                                  planes[k].plane, p)) {
          continue;
        }

        // Check point is inside all planes
        bool inside = true;
        for (const auto& fp : planes) {
          // Reject points that lie outside the convex brush.
          // A small epsilon is used to tolerate floating-point error.
          if (fp.plane.distance_to_point(p) > kInsideEps) {
            inside = false;
            break;
          }
        }

        if (!inside) {
          continue;
        }

        for (size_t f = 0; f < planes.size(); ++f) {
          if (std::abs(planes[f].plane.distance_to_point(p)) <= kOnPlaneEps) {
            Vertex v{};
            v.pos = p;
            v.norm = polys[f].normal;
            polys[f].verts.push_back(v);
          }
        }
      }
    }
  }

  return polys;
}

}  // namespace culprit::tools