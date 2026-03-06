#pragma once

#include <sstream>
#include <string>
#include <string_view>

#include "parse/MapAuthoredTypes.h"
#include "pipeline/MapBuildContext.h"

namespace culprit::tools {

namespace {

// ------------------------------------------------------------
// Helpers (copy from old prop pass style)
// ------------------------------------------------------------

inline maths::Vec3f ParseVec3(const std::string& s) {
  std::istringstream iss(s);
  float x = 0.f, y = 0.f, z = 0.f;
  iss >> x >> y >> z;
  return {x, y, z};
}

inline maths::Vec3f GetEntityVec3(const Entity& e, const char* key) {
  auto it = e.properties.find(key);
  if (it == e.properties.end())
    return {0.f, 0.f, 0.f};
  return ParseVec3(it->second);
}

inline float GetEntityFloat(const Entity& e, const char* key, float def = 1.f) {
  auto it = e.properties.find(key);
  if (it == e.properties.end())
    return def;
  return std::stof(it->second);
}

// TrenchBroom angles convention:
//   angles = pitch (X), yaw (Z), roll (Y)
// Rotation order in your old code:
//   return R_roll * R_pitch * R_yaw;
inline maths::Mat4f MakeTBRotation(const maths::Vec3f& anglesDegrees) {
  constexpr float degToRad = 3.14159265358979323846f / 180.0f;

  const float pitch = anglesDegrees.x() * degToRad;
  const float yaw   = anglesDegrees.y() * degToRad;
  const float roll  = anglesDegrees.z() * degToRad;

  const maths::Mat4f R_pitch = maths::make_rotation_x(pitch);
  const maths::Mat4f R_yaw   = maths::make_rotation_z(yaw);
  const maths::Mat4f R_roll  = maths::make_rotation_y(roll);

  return R_roll * R_pitch * R_yaw;
}

}  // namespace

struct PointEntityTransformPass {
  MapBuildContext& ctx;

  void Process(const Entity& e, uint32_t entityIndex) {
    // ==============================
    // Only point entities
    // ==============================
    if (!e.brushes.empty()) {
      return;  // brush passes will handle pivot/transform
    }

    // ==============================
    // Build TRS from TB properties
    // ==============================
    const maths::Vec3f originTB = GetEntityVec3(e, "origin");
    const maths::Vec3f angles   = GetEntityVec3(e, "angles");
    const float scale           = GetEntityFloat(e, "scale", 1.0f);

    const maths::Vec3f origin = originTB * ctx.tbToEngineScale;

    const maths::Mat4f M = maths::make_translation(origin) *
                           MakeTBRotation(angles) *
                           maths::make_scale(scale);

    // ==============================
    // Assign transform
    // ==============================
    EntityPrototype& proto = ctx.result.entities[entityIndex];
    proto.localTransform = M;
  }
};

}  // namespace culprit::tools