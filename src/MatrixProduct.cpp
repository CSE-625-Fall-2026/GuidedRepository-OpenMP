#include "guided_openmp/MatrixProduct.hpp"

#include <omp.h>

#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

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

void validateThreadCount(std::size_t matrix_size, std::size_t thread_count) {
    if (thread_count == 0 || thread_count > matrix_size) {
        throw std::invalid_argument("thread count must be between 1 and the matrix size");
    }
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

Matrix multiplyMatrices(const MatrixArray& matrices, std::size_t thread_count) {
    if (!matrices.front().isInitialized()) {
        throw std::invalid_argument("matrices must be initialized");
    }
    const std::size_t matrix_size = matrices.front().getNumRows();
    validateThreadCount(matrix_size, thread_count);
    omp_set_dynamic(0);

    Matrix result(matrix_size, matrix_size);

#pragma omp parallel num_threads(static_cast<int>(thread_count))
    {
        std::vector<double> current_values(matrix_size);
        std::vector<double> next_values(matrix_size);

#pragma omp for schedule(dynamic, 1)
        for (long long row_number = 0;
             row_number < static_cast<long long>(matrix_size);
             ++row_number) {
            const std::size_t row = static_cast<std::size_t>(row_number);
            for (std::size_t column = 0; column < matrix_size; ++column) {
                current_values[column] = matrices[0].get(row, column);
            }

            for (std::size_t index = 1; index < matrices.size(); ++index) {
                const Matrix& matrix = matrices[index];
                for (std::size_t column = 0; column < matrix_size; ++column) {
                    double sum = 0.0;
                    for (std::size_t inner = 0; inner < matrix_size; ++inner) {
                        sum += current_values[inner] * matrix.get(inner, column);
                    }
                    next_values[column] = sum;
                }
                current_values.swap(next_values);
            }

            for (std::size_t column = 0; column < matrix_size; ++column) {
                result.set(row, column, current_values[column]);
            }
        }
    }
    return result;
}

std::size_t recommendedPoolSize(std::size_t matrix_size, std::size_t thread_count) {
    validateThreadCount(matrix_size, thread_count);
    const std::size_t elements = checkedMultiply(matrix_size, matrix_size);
    const std::size_t values = checkedMultiply(elements, sizeof(double));
    const std::size_t row_overhead = checkedMultiply(matrix_size, 8192);
    const std::size_t per_matrix = checkedAdd(values, row_overhead);
    const std::size_t matrix_bytes = checkedMultiply(per_matrix, matrix_count + 1);
    const std::size_t row_buffer_elements = checkedMultiply(
        checkedMultiply(thread_count, 2),
        matrix_size
    );
    const std::size_t row_buffer_bytes = checkedMultiply(
        row_buffer_elements,
        sizeof(double)
    );
    return checkedAdd(checkedAdd(matrix_bytes, row_buffer_bytes), 64 * 1024 * 1024);
}

}
