#pragma once

#include "culprit-maths/Matrix/Matrix.hpp"
#include "culprit-maths/Vector/Vector.hpp"
#include "culprit-maths/Concepts.hpp"

namespace culprit::maths {

// Matrix x Scalar
template <Scalar T, size_t Rows, size_t Cols>
constexpr Matrix<T, Cols, Rows> operator*(const Matrix<T, Cols, Rows>& mat, T scalar) noexcept {
  Matrix<T, Cols, Rows> result;
  for (size_t row = 0; row < Rows; ++row)
    for (size_t col = 0; col < Cols; ++col) result.at(row, col) = mat.at(row, col) * scalar;
  return result;
}

template <Scalar T, size_t Cols, size_t Rows>
constexpr Matrix<T, Cols, Rows> operator*(T scalar, const Matrix<T, Cols, Rows>& m) noexcept {
  return m * scalar;
}

// Matrix / Scalar
template <Scalar T, size_t Cols, size_t Rows>
constexpr Matrix<T, Cols, Rows> operator/(const Matrix<T, Cols, Rows>& mat, T scalar) noexcept {
  Matrix<T, Cols, Rows> result;
  for (size_t row = 0; row < Rows; ++row)
    for (size_t col = 0; col < Cols; ++col) result.at(row, col) = mat.at(row, col) / scalar;
  return result;
}

// Matrix * Matrix
template <Scalar T, size_t A, size_t B, size_t C>
constexpr Matrix<T, C, A> operator*(const Matrix<T, B, A>& lhs, const Matrix<T, C, B>& rhs) noexcept {
  Matrix<T, C, A> result{};

  for (size_t row = 0; row < A; ++row) {
    for (size_t col = 0; col < C; ++col) {
      T sum{};
      for (size_t k = 0; k < B; ++k) {
        sum += lhs.at(row, k) * rhs.at(k, col);
      }
      result.at(row, col) = sum;
    }
  }

  return result;
}

// Matrix * Vector
template <Scalar T, size_t M, size_t N>
constexpr Vector<T, M> operator*(const Matrix<T, N, M>& mat, const Vector<T, N>& vec) noexcept {
  Vector<T, M> result{};

  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < N; ++col) {
      result[row] += mat.at(row, col) * vec[col];
    }
  }

  return result;
}

// Transpose
template <Scalar T, size_t Rows, size_t Cols>
constexpr Matrix<T, Rows, Cols> transpose(const Matrix<T, Cols, Rows>& mat) noexcept {
  Matrix<T, Rows, Cols> result;

  for (size_t row = 0; row < Rows; ++row)
    for (size_t col = 0; col < Cols; ++col) result.at(col, row) = mat.at(row, col);

  return result;
}

}  // namespace culprit::maths
