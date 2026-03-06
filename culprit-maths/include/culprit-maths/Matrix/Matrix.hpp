#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <culprit-maths/Concepts.hpp>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace culprit::maths {

/**
 * @brief A statically sized column-major matrix.
 *
 * @tparam T    Scalar type.
 * @tparam Cols Number of columns.
 * @tparam Rows Number of rows.
 */
template <Scalar T, size_t Cols, size_t Rows>
struct Matrix {
  T elements[Cols][Rows] = {};  // Column-major layout: elements[col][row]

  //== Constructors ==//

  /// Default constructor (zero-initialized)
  constexpr Matrix() = default;

  /// Fill all elements with a single scalar value
  constexpr explicit Matrix(T value) {
    for (size_t col = 0; col < Cols; ++col)
      for (size_t row = 0; row < Rows; ++row) elements[col][row] = value;
  }

  /// Construct from a flat column-major array of size Cols * Rows
  constexpr explicit Matrix(const T* flatData) {
    for (size_t col = 0; col < Cols; ++col)
      for (size_t row = 0; row < Rows; ++row) elements[col][row] = *flatData++;
  }

  /// Initialize matrix using nested initializer lists (row-major format)
  constexpr Matrix(std::initializer_list<std::initializer_list<T>> rowLists) {
    size_t row = 0;
    for (const auto& rowList : rowLists) {
      size_t col = 0;
      for (const auto& value : rowList) {
        if (col < Cols && row < Rows) {
          elements[col][row] = value;
        }
        ++col;
      }
      ++row;
      if (row >= Rows) break;
    }
  }

  /// Create an identity matrix (only for square matrices)
  static constexpr Matrix identity()
    requires(Cols == Rows)
  {
    Matrix m{};
    for (size_t i = 0; i < Rows; ++i) m.at(i, i) = T(1);
    return m;
  }

  //== Access ==//

  /// Returns a pointer to the matrix's first element (column-major order)
  constexpr T* data() noexcept { return &elements[0][0]; }

  /// Returns a const pointer to the matrix's first element (column-major order)
  constexpr const T* data() const noexcept { return &elements[0][0]; }

  /// Element access by row and column (mutable)
  constexpr T& at(size_t row, size_t col) noexcept {
    assert(row < Rows && col < Cols);
    return elements[col][row];
  }

  /// Element access by row and column (read-only)
  constexpr const T& at(size_t row, size_t col) const noexcept {
    assert(row < Rows && col < Cols);
    return elements[col][row];
  }

  /// Compile-time dimensions
  static constexpr size_t columns = Cols;
  static constexpr size_t rows = Rows;
};

}  // namespace culprit::maths

