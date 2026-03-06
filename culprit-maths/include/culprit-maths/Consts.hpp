#pragma once

#include <limits>
#include <cmath>
#include "Forward.hpp"

namespace culprit {
namespace maths {

// --- Scalar constants ---
template <typename T>
inline constexpr T PI = T(3.14159265358979323846);

template <typename T>
inline constexpr T TAU = T(6.28318530717958647692);

template <typename T>
inline constexpr T HALF_PI = T(1.57079632679489661923);

template <typename T>
inline constexpr T EPSILON = std::numeric_limits<T>::epsilon();

// --- Degree Radian helpers ---
template <typename T>
constexpr T radians(T degrees) noexcept {
  return degrees * (PI<T> / T(180));
}

template <typename T>
constexpr T degrees(T radians) noexcept {
  return radians * (T(180) / PI<T>);
}

}  // namespace maths
}  // namespace culprit