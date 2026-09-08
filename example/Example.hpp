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

inline bool readPositiveNumber(const char* text, std::size_t& result) {
    errno = 0;
    char* end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value == 0 ||
        value > std::numeric_limits<std::size_t>::max()) {
        return false;
    }

    result = static_cast<std::size_t>(value);
    return true;
}

inline bool readInputs(
    int argument_count,
    char** arguments,
    std::size_t& matrix_size,
    std::size_t& thread_count
) {
    if (argument_count != 3) {
        return false;
    }

    return readPositiveNumber(arguments[1], matrix_size) &&
        readPositiveNumber(arguments[2], thread_count) &&
        thread_count <= matrix_size;
}

inline double calculateChecksum(const guided_openmp::Matrix& matrix) {
    double checksum = 0.0;
    for (std::size_t index = 0; index < matrix.getNumRows(); ++index) {
        checksum += matrix.get(index, index);
    }
    return checksum;
}

inline int run(
    std::size_t matrix_size,
    std::size_t thread_count,
    const char* allocation_name
) {
    const auto start = std::chrono::steady_clock::now();
    const auto matrices = guided_openmp::createRandomMatrices(matrix_size);
    const auto product = guided_openmp::multiplyMatrices(matrices, thread_count);
    const auto finish = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed = finish - start;

    std::printf(
        "%s: %zu matrices of %zux%zu multiplied with %zu OpenMP threads in %.3f seconds, checksum %.6f\n",
        allocation_name,
        guided_openmp::matrix_count,
        matrix_size,
        matrix_size,
        thread_count,
        elapsed.count(),
        calculateChecksum(product)
    );
    return 0;
}

}

#endif
