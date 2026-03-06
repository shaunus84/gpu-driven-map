#pragma once

#include "culprit-maths/Concepts.hpp"

namespace culprit::maths {
/**
 * @brief A quaternion representing a rotation.
 * Stored in (x, y, z, w) format where (x, y, z) is the imaginary part, and w is the real part.
 */
template <Scalar T>
struct Quaternion {
  T x{}, y{}, z{}, w{1};

  constexpr Quaternion() = default;
  constexpr Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

  /// Returns the identity quaternion (no rotation).
  static constexpr Quaternion identity() { return Quaternion{T(0), T(0), T(0), T(1)}; }
};

}  // namespace culprit::maths
