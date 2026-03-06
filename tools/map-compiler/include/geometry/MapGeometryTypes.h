#pragma once

#include <vector>
#include <culprit-maths/Maths.hpp>

namespace culprit::tools {

// Temporary vertex during polygon construction
struct Vertex {
  maths::Vec3f pos{};
  maths::Vec3f norm{};
  maths::Vec2f texCoord{};
  uint32_t surfaceIndex = 0;

  constexpr bool operator==(const Vertex& v) const noexcept {
    return pos == v.pos && norm == v.norm && texCoord == v.texCoord;
  }
};

struct FacePlanes {
  maths::Planef plane;
  maths::Planef texUAxis;
  maths::Planef texVAxis;
  float uScale = 1.f;
  float vScale = 1.f;
};

struct Polygon {
  size_t faceIndex = 0;
  maths::Vec3f normal{};
  std::vector<Vertex> verts;
};

} // namespace culprit::tools
