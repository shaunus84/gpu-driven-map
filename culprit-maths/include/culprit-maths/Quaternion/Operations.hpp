#pragma once

#include <cmath>

#include "culprit-maths/Concepts.hpp"
#include "culprit-maths/Quaternion/Quaternion.hpp"

namespace culprit::maths {

template <Scalar T>
constexpr Quaternion<T> conjugate(const Quaternion<T>& q) noexcept {
  return {-q.x, -q.y, -q.z, q.w};
}

template <Scalar T>
constexpr T length_squared(const Quaternion<T>& q) noexcept {
  return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

template <Scalar T>
constexpr T length(const Quaternion<T>& q) noexcept {
  return std::sqrt(length_squared(q));
}

template <Scalar T>
constexpr Quaternion<T> normalize(const Quaternion<T>& q) noexcept {
  T len = length(q);
  return (len > T(0)) ? (q / len) : Quaternion<T>::identity();
}

template <Scalar T>
constexpr Quaternion<T> inverse(const Quaternion<T>& q) noexcept {
  return conjugate(q) / length_squared(q);
}

// --- Operators ---
template <Scalar T>
constexpr Quaternion<T> operator*(const Quaternion<T>& a, const Quaternion<T>& b) noexcept {
  return {a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
          a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
          a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
          a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

template <Scalar T>
constexpr Quaternion<T> operator*(const Quaternion<T>& q, T scalar) noexcept {
  return {q.x * scalar, q.y * scalar, q.z * scalar, q.w * scalar};
}

template <Scalar T>
constexpr Quaternion<T> operator/(const Quaternion<T>& q, T scalar) noexcept {
  return {q.x / scalar, q.y / scalar, q.z / scalar, q.w / scalar};
}

template <Scalar T>
constexpr Vector<T, 3> operator*(
    const Quaternion<T>& q,
    const Vector<T, 3>& v) noexcept
{
    // Assumes q is normalized
    const Vector<T, 3> qv{q.x, q.y, q.z};
    const Vector<T, 3> t = T(2) * cross(qv, v);
    return v + q.w * t + cross(qv, t);
}

}  // namespace culprit::maths
