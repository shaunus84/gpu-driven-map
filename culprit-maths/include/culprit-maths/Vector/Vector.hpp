#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>

#include "culprit-maths/Concepts.hpp"

namespace culprit::maths {

/**
 * @brief A fixed-size mathematical vector.
 *
 * @tparam T The scalar type (e.g., float, int).
 * @tparam N The number of components in the vector.
 */

template <Scalar T, size_t N>
struct Vector {
  T elements[N]{};  // Underlying storage

  // Constructors

  /// Default-initialized vector (zero-initialized in modern C++).
  constexpr Vector() = default;

  /// Initialize all components to the same scalar value.
  constexpr explicit Vector(T value) { std::fill_n(elements, N, value); }

  /// Variadic constructor - takes exactly N scalar arguments.
  template <typename... Args>
    requires(sizeof...(Args) == N)
  constexpr Vector(Args... args) : elements{static_cast<T>(args)...} {}

  /// Initialize from an initializer list (e.g. {1, 2, 3}).
  /// 0 if fewer than N values are provided.
  constexpr Vector(std::initializer_list<T> init) {
    using std::min;
    std::fill_n(elements, N, T(0));
    std::copy_n(init.begin(), min(N, init.size()), elements);
  }

  // Element Access

  /// Mutable access to a component by index.
  constexpr T& operator[](size_t i) noexcept { return elements[i]; }

  /// Read-only access to a component by index.
  constexpr const T& operator[](size_t i) const noexcept { return elements[i]; }

  /// Returns a pointer to the underlying data.
  constexpr T* data() noexcept { return elements; }

  /// Returns a const pointer to the underlying data.
  constexpr const T* data() const noexcept { return elements; }

  /// Number of elements in the vector.
  constexpr size_t size() const noexcept { return N; }

  // Static utility constructors

  /// Returns a vector with all components set to zero.
  static constexpr Vector zero() noexcept { return Vector(T(0)); }

  /// Returns a vector with all components set to one.
  static constexpr Vector one() noexcept { return Vector(T(1)); }

  /// Returns a unit vector pointing along the given axis.
  /// Axis must be in [0, N); otherwise, returns a zero vector.
  static constexpr Vector unit(size_t axis) noexcept {
    Vector result(T(0));
    if (axis < N) result[axis] = T(1);
    return result;
  }

  constexpr T& x() noexcept
    requires(N >= 1)
  {
    return elements[0];
  }
  constexpr T& y() noexcept
    requires(N >= 2)
  {
    return elements[1];
  }
  constexpr T& z() noexcept
    requires(N >= 3)
  {
    return elements[2];
  }
  constexpr T& w() noexcept
    requires(N >= 4)
  {
    return elements[3];
  }

  constexpr const T& x() const noexcept
    requires(N >= 1)
  {
    return elements[0];
  }
  constexpr const T& y() const noexcept
    requires(N >= 2)
  {
    return elements[1];
  }
  constexpr const T& z() const noexcept
    requires(N >= 3)
  {
    return elements[2];
  }
  constexpr const T& w() const noexcept
    requires(N >= 4)
  {
    return elements[3];
  }
};

}  // namespace culprit::maths
