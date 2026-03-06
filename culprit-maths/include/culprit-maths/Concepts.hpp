#pragma once

#include <concepts>
#include <type_traits>

namespace culprit {
namespace maths {

template <typename T>
concept Scalar = std::is_arithmetic_v<T>;

template <typename T>
concept HasIdentity = requires {
  { T::identity() } -> std::same_as<T>;
};

template <typename T>
concept HasZero = requires {
  { T::zero() } -> std::same_as<T>;
};

template <typename T>
concept HasOne = requires {
  { T::one() } -> std::same_as<T>;
};

template <typename T>
concept HasUnit = requires {
  { T::unit() } -> std::same_as<T>;
};
}  // namespace maths
}  // namespace culprit
