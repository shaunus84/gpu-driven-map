#pragma once

#include <array>
#include <culprit-maths/Maths.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace culprit::tools {

// Raw face as authored in .map
struct Face {
  std::array<maths::Vec3f, 3> points{};
  std::string materialName;
  maths::Vec4f textureUAxis{};
  maths::Vec4f textureVAxis{};
  float textureRotation = 0.f;
  float uScale = 1.f;
  float vScale = 1.f;
};

struct Brush {
  std::vector<Face> faces;
};

struct Entity {
  std::unordered_map<std::string, std::string> properties;
  std::vector<Brush> brushes;
  std::string classname;
};

}  // namespace culprit::tools
