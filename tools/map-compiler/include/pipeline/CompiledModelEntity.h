#pragma once

#include "mesh/MeshTypes.h"

namespace culprit::tools {

struct ModelEntityGeometry {
  MeshPrototype mesh;
  std::vector<uint32_t> materialSet;
};

struct CompiledModelEntity {
  MeshPrototype mesh;
  MaterialSet materialSet;
};

}  // namespace culprit::tools
