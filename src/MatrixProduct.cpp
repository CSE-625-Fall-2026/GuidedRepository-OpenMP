#include "guided_openmp/MatrixProduct.hpp"

#include <limits>
#include <random>
#include <stdexcept>

namespace guided_openmp {
namespace {

std::size_t checkedAdd(std::size_t left, std::size_t right) {
    if (right > std::numeric_limits<std::size_t>::max() - left) {
        throw std::overflow_error("matrix memory size is too large");
    }
    return left + right;
}

std::size_t checkedMultiply(std::size_t left, std::size_t right) {
    if (left != 0 && right > std::numeric_limits<std::size_t>::max() / left) {
        throw std::overflow_error("matrix memory size is too large");
    }
    return left * right;
}

}

MatrixArray createRandomMatrices(std::size_t matrix_size, std::uint32_t seed) {
    if (matrix_size == 0) {
        throw std::invalid_argument("matrix size must be greater than zero");
    }

    MatrixArray matrices;
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(
        0.0,
        2.0 / static_cast<double>(matrix_size)
    );

    for (Matrix& matrix : matrices) {
        matrix.setNumRows(matrix_size);
        matrix.setNumCols(matrix_size);
        matrix.initialize();
        for (std::size_t row = 0; row < matrix_size; ++row) {
            for (std::size_t column = 0; column < matrix_size; ++column) {
                matrix.set(row, column, distribution(generator));
            }
        }
    }
    return matrices;
}

Matrix multiplyMatrices(const MatrixArray& matrices) {
    if (!matrices.front().isInitialized()) {
        throw std::invalid_argument("matrices must be initialized");
    }

    Matrix product = matrices.front();
    for (std::size_t index = 1; index < matrices.size(); ++index) {
        product *= matrices[index];
    }
    return product;
}

std::size_t recommendedPoolSize(std::size_t matrix_size) {
    const std::size_t elements = checkedMultiply(matrix_size, matrix_size);
    const std::size_t values = checkedMultiply(elements, sizeof(double));
    const std::size_t row_overhead = checkedMultiply(matrix_size, 8192);
    const std::size_t per_matrix = checkedAdd(values, row_overhead);
    const std::size_t all_matrices = checkedMultiply(per_matrix, matrix_count + 2);
    return checkedAdd(all_matrices, 64 * 1024 * 1024);
}

}
