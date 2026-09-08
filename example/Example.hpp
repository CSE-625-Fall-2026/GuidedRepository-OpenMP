#ifndef GUIDED_REPOSITORY_OPENMP_EXAMPLE_HPP
#define GUIDED_REPOSITORY_OPENMP_EXAMPLE_HPP

#include "guided_openmp/MatrixProduct.hpp"

#include <cerrno>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <limits>

namespace example {

inline bool readMatrixSize(int argument_count, char** arguments, std::size_t& matrix_size) {
    matrix_size = guided_openmp::default_matrix_size;
    if (argument_count == 1) {
        return true;
    }
    if (argument_count != 2) {
        return false;
    }

    errno = 0;
    char* end = nullptr;
    const unsigned long long value = std::strtoull(arguments[1], &end, 10);
    if (errno != 0 || end == arguments[1] || *end != '\0' || value == 0 ||
        value > std::numeric_limits<std::size_t>::max()) {
        return false;
    }
    matrix_size = static_cast<std::size_t>(value);
    return true;
}

inline double calculateChecksum(const guided_openmp::Matrix& matrix) {
    double checksum = 0.0;
    for (std::size_t index = 0; index < matrix.getNumRows(); ++index) {
        checksum += matrix.get(index, index);
    }
    return checksum;
}

inline int run(std::size_t matrix_size, const char* allocation_name) {
    const auto start = std::chrono::steady_clock::now();
    const auto matrices = guided_openmp::createRandomMatrices(matrix_size);
    const auto product = guided_openmp::multiplyMatrices(matrices);
    const auto finish = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed = finish - start;

    std::printf(
        "%s: %zu matrices of %zux%zu multiplied in %.3f seconds, checksum %.6f\n",
        allocation_name,
        guided_openmp::matrix_count,
        matrix_size,
        matrix_size,
        elapsed.count(),
        calculateChecksum(product)
    );
    return 0;
}

}

#endif
