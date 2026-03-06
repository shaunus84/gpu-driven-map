#pragma once

namespace culprit {
namespace maths {

// Forward declarations
template <Scalar T, size_t N>
struct Vector;

template <Scalar T, size_t R, size_t C>
struct Matrix;

template <Scalar T>
struct Quaternion;

template <Scalar T>
struct Plane;

// Common aliases
using Vec2f = Vector<float, 2>;
using Vec3f = Vector<float, 3>;
using Vec4f = Vector<float, 4>;

using Vec2ui = Vector<uint32_t, 2>;
using Vec2i = Vector<int32_t, 2>;

using Mat4f = Matrix<float, 4, 4>;

using Quatf = Quaternion<float>;

using Planef = Plane<float>;

}  // namespace maths
}  // namespace culprit
