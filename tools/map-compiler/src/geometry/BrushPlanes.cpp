#include "geometry/BrushPlanes.h"

// This file canonicalises brush face planes for CSG processing.
// All planes are oriented such that the interior of the brush lies
// on the non-negative side of each plane.

#include <culprit-maths/Maths.hpp>

namespace culprit::tools {

// Builds one FacePlanes entry per authored face in the brush.
std::vector<FacePlanes> BuildFacePlanes(const Brush& brush) {
  std::vector<FacePlanes> planes;
  planes.reserve(brush.faces.size());

  for (const Face& f : brush.faces) {
    FacePlanes fp{};

    // Trust TrenchBroom winding: planes already face outward
    fp.plane = maths::Planef::from_points(
        f.points[0], f.points[1], f.points[2]);

    fp.texUAxis = maths::Planef(f.textureUAxis[0], f.textureUAxis[1],
                                f.textureUAxis[2], f.textureUAxis[3]);
    fp.texVAxis = maths::Planef(f.textureVAxis[0], f.textureVAxis[1],
                                f.textureVAxis[2], f.textureVAxis[3]);

    fp.uScale = f.uScale;
    fp.vScale = f.vScale;

    planes.push_back(fp);
  }

  return planes;
}


}  // namespace culprit::tools