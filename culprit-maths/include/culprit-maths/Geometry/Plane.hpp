#pragma once

#include "culprit-maths/Concepts.hpp"
#include "culprit-maths/Vector/Operations.hpp"
#include "culprit-maths/Vector/Vector.hpp"

namespace culprit {
namespace maths {

template <Scalar T>
struct Plane {
  Vector<T, 3> normal{T(0), T(1), T(0)};
  T distance{0};  // Distance from origin along normal

  // --- Constructors ---
  constexpr Plane() = default;

  constexpr Plane(const Vector<T, 3>& normal, T distance) noexcept
      : normal(normal), distance(distance) {}

  constexpr Plane(T a, T b, T c, T d) noexcept : normal{a, b, c}, distance(d) {}

  // Construct from point and normal
  static constexpr Plane from_point_normal(
      const Vector<T, 3>& point, const Vector<T, 3>& normal) noexcept {
    auto n = normalized(normal);
    return {n, -dot(n, point)};
  }

  // Construct from 3 points
  static Plane from_points(const Vector<T, 3>& p0, const Vector<T, 3>& p1,
                           const Vector<T, 3>& p2) noexcept {
    auto n = normalized(cross(p2 - p0, p1 - p0));
    return from_point_normal(p0, n);
  }

  static Plane from_points_oriented(
      const Vector<T, 3>& p0, const Vector<T, 3>& p1, const Vector<T, 3>& p2,
      const Vector<T, 3>& interiorPoint) noexcept {
    Plane p = from_points(p0, p1, p2);

    if (p.distance_to_point(interiorPoint) > T(0)) {
      p.flip();
    }
    return p;
  }

  // --- Evaluation ---
  constexpr T distance_to_point(const Vector<T, 3>& point) const noexcept {
    return dot(normal, point) + distance;
  }

  constexpr bool is_front_facing_to(
      const Vector<T, 3>& direction) const noexcept {
    return dot(normal, direction) <= T(0);
  }

  // --- Utility ---
  constexpr Plane flipped() const noexcept { return Plane{-normal, -distance}; }
  constexpr void flip() noexcept {
    normal = -normal;
    distance = -distance;
  }

  // --- Comparison ---
  constexpr bool operator==(const Plane& rhs) const noexcept {
    return normal == rhs.normal && distance == rhs.distance;
  }

  constexpr bool operator!=(const Plane& rhs) const noexcept {
    return !(*this == rhs);
  }
};

}  // namespace maths
}  // namespace culprit
