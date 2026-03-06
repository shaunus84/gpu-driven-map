#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>

#include "culprit-maths/Consts.hpp"
#include "culprit-maths/Matrix/Matrix.hpp"
#include "culprit-maths/Quaternion/Quaternion.hpp"
#include "culprit-maths/Vector/Vector.hpp"

namespace culprit::maths {

// --- Euler -> Quaternion (XYZ) ---
template <std::floating_point T>
constexpr Quaternion<T> from_euler_xyz(const Vector<T, 3>& a) noexcept {
  T cx = std::cos(a[0] * T(0.5));
  T sx = std::sin(a[0] * T(0.5));
  T cy = std::cos(a[1] * T(0.5));
  T sy = std::sin(a[1] * T(0.5));
  T cz = std::cos(a[2] * T(0.5));
  T sz = std::sin(a[2] * T(0.5));

  return {sx * cy * cz - cx * sy * sz,
          cx * sy * cz + sx * cy * sz,
          cx * cy * sz - sx * sy * cz,
          cx * cy * cz + sx * sy * sz};
}

template <std::floating_point T>
constexpr Vector<T, 3> to_euler_xyz(const Quaternion<T>& q) noexcept {
  // X (pitch):
  T sinp = T(2) * (q.w * q.x + q.y * q.z);
  T cosp = T(1) - T(2) * (q.x * q.x + q.y * q.y);
  T pitch = std::atan2(sinp, cosp);

  // Y (yaw):
  T siny = T(2) * (q.w * q.y - q.z * q.x);
  T yaw = std::clamp(siny, T(-1), T(1));
  yaw = std::asin(yaw);

  // Z (roll):
  T sinr = T(2) * (q.w * q.z + q.x * q.y);
  T cosr = T(1) - T(2) * (q.y * q.y + q.z * q.z);
  T roll = std::atan2(sinr, cosr);

  return {pitch, yaw, roll};
}

template <std::floating_point T>
constexpr Vector<T, 3> to_euler_xyz_degrees(const Quaternion<T>& q) noexcept {
  auto r = to_euler_xyz(q);
  return {r[0] * T(180) / PI<T>, r[1] * T(180) / PI<T>, r[2] * T(180) / PI<T>};
}

// --- Quaternion -> Matrix3 ---
template <std::floating_point T>
constexpr Matrix<T, 3, 3> to_matrix3(const Quaternion<T>& q) noexcept {
  T xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
  T xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
  T wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

  return {{{1 - 2 * (yy + zz), 2 * (xy - wz), 2 * (xz + wy)},
           {2 * (xy + wz), 1 - 2 * (xx + zz), 2 * (yz - wx)},
           {2 * (xz - wy), 2 * (yz + wx), 1 - 2 * (xx + yy)}}};
}

// --- Quaternion -> Matrix4 (for TRS)
template <std::floating_point T>
constexpr Matrix<T, 4, 4> to_matrix4(const Quaternion<T>& q) noexcept {
  Matrix<T, 4, 4> m = Matrix<T, 4, 4>::identity();
  auto r = to_matrix3(q);
  for (size_t row = 0; row < 3; ++row)
    for (size_t col = 0; col < 3; ++col) m.at(row, col) = r.at(row, col);
  return m;
}

}  // namespace culprit::maths
