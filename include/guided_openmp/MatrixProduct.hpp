#ifndef GUIDED_REPOSITORY_OPENMP_MATRIX_PRODUCT_HPP
#define GUIDED_REPOSITORY_OPENMP_MATRIX_PRODUCT_HPP

#include <matrix/Matrix.hpp>

#include <array>
#include <cstddef>
#include <cstdint>

namespace guided_openmp {

using Matrix = matrix::Matrix<double>;
constexpr std::size_t matrix_count = 200;
constexpr std::uint32_t default_seed = 625;
using MatrixArray = std::array<Matrix, matrix_count>;

MatrixArray createRandomMatrices(std::size_t matrix_size, std::uint32_t seed = default_seed);
std::size_t rowsForThread(
    std::size_t row_count,
    std::size_t thread_index,
    std::size_t thread_count
);
std::size_t startingRowForThread(
    std::size_t row_count,
    std::size_t thread_index,
    std::size_t thread_count
);
Matrix multiplyMatrices(const MatrixArray& matrices, std::size_t thread_count);
std::size_t recommendedPoolSize(std::size_t matrix_size, std::size_t thread_count);

}

#endif
