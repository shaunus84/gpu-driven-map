#pragma once

#include "culprit-maths/Matrix/Matrix.hpp"

namespace culprit {
namespace maths {

template <typename T>
Matrix<T, 2, 2> inverse(const Matrix<T, 2, 2>& m) {
  const T det = m.at(0, 0) * m.at(1, 1) - m.at(0, 1) * m.at(1, 0);
  if (det == T(0)) return {};  // Not invertible

  const T invDet = T(1) / det;

  return {{m.at(1, 1) * invDet, -m.at(0, 1) * invDet}, {-m.at(1, 0) * invDet, m.at(0, 0) * invDet}};
}

template <typename T>
Matrix<T, 3, 3> inverse(const Matrix<T, 3, 3>& m) {
  const T a = m.at(0, 0), b = m.at(0, 1), c = m.at(0, 2);
  const T d = m.at(1, 0), e = m.at(1, 1), f = m.at(1, 2);
  const T g = m.at(2, 0), h = m.at(2, 1), i = m.at(2, 2);

  const T A = e * i - f * h;
  const T B = -(d * i - f * g);
  const T C = d * h - e * g;
  const T D = -(b * i - c * h);
  const T E = a * i - c * g;
  const T F = -(a * h - b * g);
  const T G = b * f - c * e;
  const T H = -(a * f - c * d);
  const T I = a * e - b * d;

  const T det = a * A + b * B + c * C;
  if (det == T(0)) return {};

  const T invDet = T(1) / det;

  return {{A * invDet, D * invDet, G * invDet},
          {B * invDet, E * invDet, H * invDet},
          {C * invDet, F * invDet, I * invDet}};
}

template <typename T>
Matrix<T, 4, 4> inverse(const Matrix<T, 4, 4>& m) {
  const T* a = m.data();  // flat column-major read
  Matrix<T, 4, 4> inv{};
  T* o = inv.data();  // flat column-major write

  o[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] + a[9] * a[7] * a[14] +
         a[13] * a[6] * a[11] - a[13] * a[7] * a[10];

  o[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] - a[8] * a[7] * a[14] -
         a[12] * a[6] * a[11] + a[12] * a[7] * a[10];

  o[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] + a[8] * a[7] * a[13] + a[12] * a[5] * a[11] -
         a[12] * a[7] * a[9];

  o[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] - a[8] * a[6] * a[13] -
          a[12] * a[5] * a[10] + a[12] * a[6] * a[9];

  o[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] -
         a[13] * a[2] * a[11] + a[13] * a[3] * a[10];

  o[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] +
         a[12] * a[2] * a[11] - a[12] * a[3] * a[10];

  o[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] -
         a[12] * a[1] * a[11] + a[12] * a[3] * a[9];

  o[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] +
          a[12] * a[1] * a[10] - a[12] * a[2] * a[9];

  o[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] + a[13] * a[2] * a[7] -
         a[13] * a[3] * a[6];

  o[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] - a[12] * a[2] * a[7] +
         a[12] * a[3] * a[6];

  o[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] + a[12] * a[1] * a[7] -
          a[12] * a[3] * a[5];

  o[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] - a[12] * a[1] * a[6] +
          a[12] * a[2] * a[5];

  o[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] - a[9] * a[2] * a[7] +
         a[9] * a[3] * a[6];

  o[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] + a[8] * a[2] * a[7] -
         a[8] * a[3] * a[6];

  o[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] - a[8] * a[1] * a[7] +
          a[8] * a[3] * a[5];

  o[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] + a[8] * a[1] * a[6] -
          a[8] * a[2] * a[5];

  T det = a[0] * o[0] + a[1] * o[4] + a[2] * o[8] + a[3] * o[12];
  if (det == T(0)) return {};

  T invDet = T(1) / det;
  for (int i = 0; i < 16; ++i) o[i] *= invDet;

  return inv;
}
}  // namespace maths
}  // namespace culprit