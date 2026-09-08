#include "Example.hpp"

#include <custom_memory/GlobalNew.hpp>

#include <cstddef>
#include <cstdio>
#include <exception>

int main(int argument_count, char** arguments) {
    std::size_t matrix_size = 0;
    if (!example::readMatrixSize(argument_count, arguments, matrix_size)) {
        std::fprintf(stderr, "usage: %s [MATRIX_SIZE]\n", arguments[0]);
        return 1;
    }

    try {
        const std::size_t pool_size = guided_openmp::recommendedPoolSize(matrix_size);
        if (!custom_memory::initialize(pool_size)) {
            std::fprintf(stderr, "unable to initialize custom memory\n");
            return 1;
        }
        const int result = example::run(matrix_size, "overloaded new v7");
        if (!custom_memory::shutdown()) {
            std::fprintf(stderr, "unable to release custom memory\n");
            return 1;
        }
        return result;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
