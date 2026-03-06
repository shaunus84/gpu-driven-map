#include "mesh/EntityMeshBuilder.h"

#include <cmath>
#include <cstdint>
#include <culprit-tools-shared/MapIntermediateFormat.hpp>
#include <limits>
#include <vector>
#include <iostream>

#include "geometry/BrushCSG.h"
#include "geometry/BrushPlanes.h"
#include "geometry/PolygonUtils.h"
#include "geometry/UVUtils.h"

namespace culprit::tools {

namespace {

constexpr float kMaxFloat = std::numeric_limits<float>::max();

// Accumulate centroid incrementally (double precision)
struct CentroidAccumulator {
  double sx = 0.0;
  double sy = 0.0;
  double sz = 0.0;
  uint32_t count = 0;

  void add(const formats::MapVertex& v) {
    sx += double(v.px);
    sy += double(v.py);
    sz += double(v.pz);
    ++count;
  }

  maths::Vec3f finalize() const {
    if (count == 0)
      return maths::Vec3f{0.f, 0.f, 0.f};
    const double inv = 1.0 / double(count);
    return maths::Vec3f{float(sx * inv), float(sy * inv), float(sz * inv)};
  }
};

}  // namespace

// ------------------------------------------------------------
// CompileBrushEntityWorld
// ------------------------------------------------------------
CompiledBrushEntity CompileBrushEntityWorld(const Entity& entity, float scale) {
  CompiledBrushEntity out{};

  MeshPrototype& mesh = out.mesh;
  MaterialSet& matSet = out.materialSet;

  uint32_t baseVertex = 0;
  uint32_t nextSurfaceIndex = 0;

  // Heuristic reserves to reduce realloc churn
  mesh.vertices.reserve(entity.brushes.size() * 64);
  mesh.indices.reserve(entity.brushes.size() * 128);
  matSet.materials.reserve(entity.brushes.size() * 8);

  CentroidAccumulator centroid;

  for (const Brush& brush : entity.brushes) {
    const std::vector<FacePlanes> planes = BuildFacePlanes(brush);
    std::vector<Polygon> polygons = BuildBrushPolygons(planes);

    for (Polygon& poly : polygons) {
      if (!ValidateRawPolygon(poly))
        continue;

      SortVertsCW(poly.verts, poly.normal);
      DedupeVertsEpsilon(poly.verts);
      if (!ValidateLoopPolygon(poly))
        continue;

      for (auto& v : poly.verts) {
        v.norm = poly.normal;
      }

      const FacePlanes& fp = planes[poly.faceIndex];

      // Texcoords computed in world space
      CalculateTexcoordsTexelSpace(fp.texUAxis, fp.texVAxis, fp.uScale, fp.vScale, poly.verts);

      const uint32_t surfaceIndex = nextSurfaceIndex++;
      matSet.materials.push_back(brush.faces[poly.faceIndex].materialName);

      const uint32_t localVertCount = static_cast<uint32_t>(poly.verts.size());

      // Emit vertices
      for (const auto& v : poly.verts) {
        formats::MapVertex mv{};
        mv.px = v.pos.x() * scale;
        mv.py = v.pos.y() * scale;
        mv.pz = v.pos.z() * scale;
        mv.nx = v.norm.x();
        mv.ny = v.norm.y();
        mv.nz = v.norm.z();
        mv.u = v.texCoord.x();
        mv.v = v.texCoord.y();
        mv.surfaceIndex = static_cast<float>(surfaceIndex);

        centroid.add(mv);
        mesh.vertices.push_back(mv);
      }

      // Tri fan (fixed winding)
      for (uint32_t i = 1; i + 1 < localVertCount; ++i) {
        mesh.indices.push_back(baseVertex);
        mesh.indices.push_back(baseVertex + i + 1);
        mesh.indices.push_back(baseVertex + i);
      }

      baseVertex += localVertCount;
    }
  }

  mesh.surfaceCount = static_cast<uint32_t>(matSet.materials.size());

  // Canonical pivot = centroid of world-space vertices
  out.pivot = centroid.finalize();

  return out;
}

// ------------------------------------------------------------
// CompileBrushEntity
// ------------------------------------------------------------
CompiledBrushEntity CompileBrushEntity(const Entity& entity, float scale) {
  CompiledBrushEntity out = CompileBrushEntityWorld(entity, scale);

  float minX = kMaxFloat, minY = kMaxFloat, minZ = kMaxFloat;
  float maxX = -kMaxFloat, maxY = -kMaxFloat, maxZ = -kMaxFloat;

  for (const auto& v : out.mesh.vertices) {
    minX = std::min(minX, v.px);
    minY = std::min(minY, v.py);
    minZ = std::min(minZ, v.pz);

    maxX = std::max(maxX, v.px);
    maxY = std::max(maxY, v.py);
    maxZ = std::max(maxZ, v.pz);
  }

  std::cout << "AABB:\n";
  std::cout << "  X: " << minX << " -> " << maxX << "\n";
  std::cout << "  Y: " << minY << " -> " << maxY << "\n";
  std::cout << "  Z: " << minZ << " -> " << maxZ << "\n";

  const maths::Vec3f pivot = out.pivot;
  for (auto& mv : out.mesh.vertices) {
    mv.px -= pivot.x();
    mv.py -= pivot.y();
    mv.pz -= pivot.z();
  }

  return out;
}

}  // namespace culprit::tools
