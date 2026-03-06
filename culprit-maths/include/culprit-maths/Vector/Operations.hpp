#pragma once

#include <cmath>
#include <algorithm>

#include "culprit-maths/Concepts.hpp"
#include "culprit-maths/Consts.hpp"
#include "culprit-maths/Vector/Vector.hpp"

namespace culprit::maths {

// Negation
template <Scalar T, size_t N>
constexpr Vector<T, N> operator-(const Vector<T, N>& v) noexcept {
  Vector<T, N> result;
  for (size_t i = 0; i < N; ++i) result[i] = -v[i];
  return result;
}

// Add
template <Scalar T, size_t N>
constexpr Vector<T, N> operator+(const Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  Vector<T, N> result;
  for (size_t i = 0; i < N; ++i) result[i] = a[i] + b[i];
  return result;
}

// Subtract
template <Scalar T, size_t N>
constexpr Vector<T, N> operator-(const Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  Vector<T, N> result;
  for (size_t i = 0; i < N; ++i) result[i] = a[i] - b[i];
  return result;
}

// Scalar multiply
template <Scalar T, size_t N>
constexpr Vector<T, N> operator*(const Vector<T, N>& v, T scalar) noexcept {
  Vector<T, N> result;
  for (size_t i = 0; i < N; ++i) result[i] = v[i] * scalar;
  return result;
}

// Scalar multiply
template <Scalar T, size_t N>
constexpr Vector<T, N> operator*(T scalar, const Vector<T, N>& v) noexcept {
  return v * scalar;
}

// Scalar divide
template <Scalar T, size_t N>
constexpr Vector<T, N> operator/(const Vector<T, N>& v, T scalar) noexcept {
  Vector<T, N> result;
  for (size_t i = 0; i < N; ++i) result[i] = v[i] / scalar;
  return result;
}

// Compound ops
template <Scalar T, size_t N>
constexpr Vector<T, N>& operator+=(Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  for (size_t i = 0; i < N; ++i) a[i] += b[i];
  return a;
}

template <Scalar T, size_t N>
constexpr Vector<T, N>& operator-=(Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  for (size_t i = 0; i < N; ++i) a[i] -= b[i];
  return a;
}

template <Scalar T, size_t N>
constexpr Vector<T, N>& operator*=(Vector<T, N>& a, T scalar) noexcept {
  for (size_t i = 0; i < N; ++i) a[i] *= scalar;
  return a;
}

template <Scalar T, size_t N>
constexpr Vector<T, N>& operator/=(Vector<T, N>& a, T scalar) noexcept {
  for (size_t i = 0; i < N; ++i) a[i] /= scalar;
  return a;
}

// Equality
template <Scalar T, size_t N>
constexpr bool operator==(const Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  for (size_t i = 0; i < N; ++i)
    if (a[i] != b[i]) return false;
  return true;
}

template <Scalar T, size_t N>
constexpr bool operator!=(const Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  return !(a == b);
}

// Math ops
template <Scalar T, size_t N>
constexpr T dot(const Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  T sum = T(0);
  for (size_t i = 0; i < N; ++i) sum += a[i] * b[i];
  return sum;
}

template <Scalar T>
constexpr Vector<T, 3> cross(const Vector<T, 3>& a, const Vector<T, 3>& b) noexcept {
  return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}

template <Scalar T, size_t N>
constexpr T length_squared(const Vector<T, N>& v) noexcept {
  return dot(v, v);
}

template <Scalar T, size_t N>
  requires std::floating_point<T>
constexpr T length(const Vector<T, N>& v) noexcept {
  return std::sqrt(length_squared(v));
}

template <Scalar T, size_t N>
  requires std::floating_point<T>
Vector<T, N> normalized(const Vector<T, N>& v) noexcept {
  const T len = length(v);
  return (len != T(0)) ? (v / len) : Vector<T, N>::zero();
}

template <Scalar T, size_t N>
  requires std::floating_point<T>
void normalize(Vector<T, N>& v) noexcept {
  v = normalized(v);
}

template <Scalar T, size_t N>
constexpr Vector<T, N> radians(const Vector<T, N>& degreesVec) noexcept {
  return degreesVec * (PI<T> / T(180));
}

template <Scalar T, size_t N>
constexpr Vector<T, N> degrees(const Vector<T, N>& radiansVec) noexcept {
  return radiansVec * (T(180) / PI<T>);
}

// Component-wise min / max
template <Scalar T, size_t N>
constexpr Vector<T, N> min(const Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  Vector<T, N> result;
  for (size_t i = 0; i < N; ++i)
    result[i] = std::min(a[i], b[i]);
  return result;
}

template <Scalar T, size_t N>
constexpr Vector<T, N> max(const Vector<T, N>& a, const Vector<T, N>& b) noexcept {
  Vector<T, N> result;
  for (size_t i = 0; i < N; ++i)
    result[i] = std::max(a[i], b[i]);
  return result;
}


}  // namespace culprit::maths
