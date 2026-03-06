#pragma once

#include <cmath>
#include <concepts>

#include "culprit-maths/Matrix/Matrix.hpp"
#include "culprit-maths/Vector/Operations.hpp"
#include "culprit-maths/Vector/Vector.hpp"

namespace culprit::maths {

template <std::floating_point T>
using Mat4T = Matrix<T, 4, 4>;

template <std::floating_point T>
using Vec3T = Vector<T, 3>;

// Translation
template <std::floating_point T>
constexpr Mat4T<T> make_translation(const Vec3T<T>& t) noexcept {
  Mat4T<T> result = Mat4T<T>::identity();
  result.at(0, 3) = t[0];
  result.at(1, 3) = t[1];
  result.at(2, 3) = t[2];
  return result;
}

// Scale
template <std::floating_point T>
constexpr Mat4T<T> make_scale(const Vec3T<T>& s) noexcept {
  Mat4T<T> result = Mat4T<T>::identity();
  result.at(0, 0) = s[0];
  result.at(1, 1) = s[1];
  result.at(2, 2) = s[2];
  return result;
}

// Scale Uniform
template <std::floating_point T>
constexpr Mat4T<T> make_scale(T s) noexcept {
  Mat4T<T> result = Mat4T<T>::identity();
  result.at(0, 0) = s;
  result.at(1, 1) = s;
  result.at(2, 2) = s;
  return result;
}

// Rotation around X axis
template <std::floating_point T>
constexpr Mat4T<T> make_rotation_x(T radians) noexcept {
  Mat4T<T> result = Mat4T<T>::identity();
  T c = std::cos(radians);
  T s = std::sin(radians);

  result.at(1, 1) = c;
  result.at(1, 2) = -s;
  result.at(2, 1) = s;
  result.at(2, 2) = c;

  return result;
}

// Rotation around Y axis
template <std::floating_point T>
constexpr Mat4T<T> make_rotation_y(T radians) noexcept {
  Mat4T<T> result = Mat4T<T>::identity();
  T c = std::cos(radians);
  T s = std::sin(radians);

  result.at(0, 0) = c;
  result.at(0, 2) = s;
  result.at(2, 0) = -s;
  result.at(2, 2) = c;

  return result;
}

// Rotation around Z axis
template <std::floating_point T>
constexpr Mat4T<T> make_rotation_z(T radians) noexcept {
  Mat4T<T> result = Mat4T<T>::identity();
  T c = std::cos(radians);
  T s = std::sin(radians);

  result.at(0, 0) = c;
  result.at(0, 1) = -s;
  result.at(1, 0) = s;
  result.at(1, 1) = c;

  return result;
}

template <std::floating_point T>
constexpr Mat4T<T> look_at(
    const Vec3T<T>& eye,
    const Vec3T<T>& target,
    const Vec3T<T>& worldUp) noexcept
{
    // World forward (your engine: +Y)
    const Vec3T<T> f = normalized(target - eye);

    // Right-handed camera basis
    const Vec3T<T> r = normalized(cross(f, worldUp));
    const Vec3T<T> u = cross(r, f);

    Mat4T<T> view = Mat4T<T>::identity();

    // Column-vector convention
    // Rows of R^T

    // Row 0 = right
    view.at(0,0) = r.x();
    view.at(0,1) = r.y();
    view.at(0,2) = r.z();

    // Row 1 = up
    view.at(1,0) = u.x();
    view.at(1,1) = u.y();
    view.at(1,2) = u.z();

    // Row 2 = -forward   (IMPORTANT: view forward is -Z)
    view.at(2,0) = -f.x();
    view.at(2,1) = -f.y();
    view.at(2,2) = -f.z();

    // Translation = -R^T * eye
    view.at(0,3) = -dot(r, eye);
    view.at(1,3) = -dot(u, eye);
    view.at(2,3) =  dot(f, eye);

    return view;
}

template <std::floating_point T>
constexpr Mat4T<T> perspective(T fovYRadians, T aspect, T nearZ, T farZ,
                               bool homogeneousDepth) noexcept {
  assert(aspect > T(0));
  assert(farZ > nearZ);

  const T f = T(1) / std::tan(fovYRadians * T(0.5));

  Mat4T<T> m{};
  m.at(0, 0) = f / aspect;
  m.at(1, 1) = f;

  if (homogeneousDepth) {
    // OpenGL-style: z E [-1, 1]
    m.at(2, 2) = (farZ + nearZ) / (nearZ - farZ);
    m.at(2, 3) = (T(2) * farZ * nearZ) / (nearZ - farZ);
    m.at(3, 2) = T(-1);
  } else {
    // D3D-style: z E [0, 1]
    m.at(2, 2) = farZ / (nearZ - farZ);
    m.at(2, 3) = (farZ * nearZ) / (nearZ - farZ);
    m.at(3, 2) = T(-1);
  }

  m.at(3, 3) = T(0);
  return m;
}

// Orthographic projection
template <std::floating_point T>
constexpr Mat4T<T> orthographic(T left, T right, T bottom, T top, T nearZ, T farZ,
                                bool homogeneousDepth) noexcept {
  Mat4T<T> m = Mat4T<T>::identity();

  // Scale
  m.at(0, 0) = T(2) / (right - left);
  m.at(1, 1) = T(2) / (top - bottom);

  if (homogeneousDepth) {
    // OpenGL-style: z E [-1, 1]
    m.at(2, 2) = T(2) / (nearZ - farZ);
    m.at(2, 3) = (farZ + nearZ) / (nearZ - farZ);
  } else {
    // D3D-style: z E [0, 1]
    m.at(2, 2) = T(1) / (nearZ - farZ);
    m.at(2, 3) = nearZ / (nearZ - farZ);
  }

  // Translation
  m.at(0, 3) = -(right + left) / (right - left);
  m.at(1, 3) = -(top + bottom) / (top - bottom);

  return m;
}

}  // namespace culprit::maths
