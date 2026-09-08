#include "guided_openmp/MatrixProduct.hpp"

#include <omp.h>

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

void validateThread(
    std::size_t row_count,
    std::size_t thread_index,
    std::size_t thread_count
) {
    if (thread_count == 0 || thread_count > row_count) {
        throw std::invalid_argument("thread count must be between 1 and the matrix size");
    }
    if (thread_index >= thread_count) {
        throw std::out_of_range("thread index is outside the thread count");
    }
}

Matrix multiplyPair(const Matrix& left, const Matrix& right, std::size_t thread_count) {
    Matrix result(left.getNumRows(), right.getNumCols());

#pragma omp parallel for num_threads(static_cast<int>(thread_count)) schedule(static)
    for (long long row_number = 0;
         row_number < static_cast<long long>(left.getNumRows());
         ++row_number) {
        const std::size_t row = static_cast<std::size_t>(row_number);
        for (std::size_t column = 0; column < right.getNumCols(); ++column) {
            double sum = 0.0;
            for (std::size_t inner = 0; inner < left.getNumCols(); ++inner) {
                sum += left.get(row, inner) * right.get(inner, column);
            }
            result.set(row, column, sum);
        }
    }
    return result;
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

std::size_t rowsForThread(
    std::size_t row_count,
    std::size_t thread_index,
    std::size_t thread_count
) {
    validateThread(row_count, thread_index, thread_count);
    const std::size_t even_share = row_count / thread_count;
    const std::size_t extra_rows = row_count % thread_count;
    return even_share + (thread_index < extra_rows ? 1 : 0);
}

std::size_t startingRowForThread(
    std::size_t row_count,
    std::size_t thread_index,
    std::size_t thread_count
) {
    validateThread(row_count, thread_index, thread_count);
    const std::size_t even_share = row_count / thread_count;
    const std::size_t extra_rows = row_count % thread_count;
    return thread_index * even_share +
        (thread_index < extra_rows ? thread_index : extra_rows);
}

Matrix multiplyMatrices(const MatrixArray& matrices, std::size_t thread_count) {
    if (!matrices.front().isInitialized()) {
        throw std::invalid_argument("matrices must be initialized");
    }
    const std::size_t matrix_size = matrices.front().getNumRows();
    validateThread(matrix_size, 0, thread_count);
    omp_set_dynamic(0);

    Matrix product = matrices.front();
    for (std::size_t index = 1; index < matrices.size(); ++index) {
        product = multiplyPair(product, matrices[index], thread_count);
    }
    return product;
}

std::size_t recommendedPoolSize(std::size_t matrix_size, std::size_t thread_count) {
    validateThread(matrix_size, 0, thread_count);
    const std::size_t elements = checkedMultiply(matrix_size, matrix_size);
    const std::size_t values = checkedMultiply(elements, sizeof(double));
    const std::size_t row_overhead = checkedMultiply(matrix_size, 8192);
    const std::size_t per_matrix = checkedAdd(values, row_overhead);
    const std::size_t working_matrices = checkedAdd(matrix_count, 2);
    const std::size_t all_matrices = checkedMultiply(per_matrix, working_matrices);
    return checkedAdd(all_matrices, 64 * 1024 * 1024);
}

}
