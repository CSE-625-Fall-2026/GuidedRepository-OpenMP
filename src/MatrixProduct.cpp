#include "guided_openmp/MatrixProduct.hpp"

#include <omp.h>

#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
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

void validateThread(std::size_t thread_index, std::size_t thread_count) {
    if (thread_count == 0 || thread_count > matrix_count) {
        throw std::invalid_argument("thread count must be between 1 and 200");
    }
    if (thread_index >= thread_count) {
        throw std::out_of_range("thread index is outside the thread count");
    }
}

struct ThreadWork {
    std::size_t start{0};
    std::size_t count{0};
    Matrix product;
};

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

std::size_t matricesForThread(std::size_t thread_index, std::size_t thread_count) {
    validateThread(thread_index, thread_count);
    const std::size_t even_share = matrix_count / thread_count;
    const std::size_t extra_matrices = matrix_count % thread_count;
    return even_share + (thread_index < extra_matrices ? 1 : 0);
}

std::size_t startingMatrixForThread(std::size_t thread_index, std::size_t thread_count) {
    validateThread(thread_index, thread_count);
    const std::size_t even_share = matrix_count / thread_count;
    const std::size_t extra_matrices = matrix_count % thread_count;
    return thread_index * even_share +
        (thread_index < extra_matrices ? thread_index : extra_matrices);
}

Matrix multiplyMatrices(const MatrixArray& matrices, std::size_t thread_count) {
    if (!matrices.front().isInitialized()) {
        throw std::invalid_argument("matrices must be initialized");
    }
    validateThread(0, thread_count);

    std::vector<ThreadWork> work(thread_count);
    omp_set_dynamic(0);

#pragma omp parallel for num_threads(static_cast<int>(thread_count)) schedule(static, 1)
    for (long long chunk = 0; chunk < static_cast<long long>(thread_count); ++chunk) {
        const std::size_t index = static_cast<std::size_t>(chunk);
        work[index].start = startingMatrixForThread(index, thread_count);
        work[index].count = matricesForThread(index, thread_count);
        work[index].product = matrices[work[index].start];

        const std::size_t end = work[index].start + work[index].count;
        for (std::size_t matrix_index = work[index].start + 1;
             matrix_index < end;
             ++matrix_index) {
            work[index].product *= matrices[matrix_index];
        }
    }

    Matrix product = std::move(work.front().product);
    for (std::size_t index = 1; index < work.size(); ++index) {
        product *= work[index].product;
    }
    return product;
}

std::size_t recommendedPoolSize(std::size_t matrix_size, std::size_t thread_count) {
    validateThread(0, thread_count);
    const std::size_t elements = checkedMultiply(matrix_size, matrix_size);
    const std::size_t values = checkedMultiply(elements, sizeof(double));
    const std::size_t row_overhead = checkedMultiply(matrix_size, 8192);
    const std::size_t per_matrix = checkedAdd(values, row_overhead);
    const std::size_t working_matrices = checkedAdd(
        matrix_count,
        checkedMultiply(thread_count, 2)
    );
    const std::size_t all_matrices = checkedMultiply(per_matrix, working_matrices);
    return checkedAdd(all_matrices, 64 * 1024 * 1024);
}

}
