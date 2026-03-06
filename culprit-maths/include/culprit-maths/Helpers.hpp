#pragma once

#include "Concepts.hpp"

namespace culprit {
namespace maths {
template <typename T>
constexpr T identity()
  requires HasIdentity<T>
{
  return T::identity();
}

template <typename T>
constexpr T zero()
  requires HasZero<T>
{
  return T::zero();
}

template <typename T>
constexpr T one()
  requires HasOne<T>
{
  return T::one();
}
}  // namespace maths
}  // namespace culprit