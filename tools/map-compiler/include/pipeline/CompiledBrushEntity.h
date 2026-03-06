#pragma once

#include "mesh/MeshTypes.h"

namespace culprit::tools {

struct CompiledBrushEntity {
  MeshPrototype mesh;
  MaterialSet materialSet;
  maths::Vec3f pivot;
};

} // namespace culprit::tools
