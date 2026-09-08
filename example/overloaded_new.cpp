#include "Example.hpp"

#include <custom_memory/GlobalNew.hpp>

#include <cstddef>
#include <cstdio>
#include <exception>

int main(int argument_count, char** arguments) {
    std::size_t matrix_size = 0;
    std::size_t thread_count = 0;
    if (!example::readInputs(argument_count, arguments, matrix_size, thread_count)) {
        std::fprintf(stderr, "usage: %s MATRIX_SIZE THREAD_COUNT\n", arguments[0]);
        return 1;
    }

    try {
        const std::size_t pool_size = guided_openmp::recommendedPoolSize(matrix_size, thread_count);
        if (!custom_memory::initialize(pool_size)) {
            std::fprintf(stderr, "unable to initialize custom memory\n");
            return 1;
        }
        return example::run(matrix_size, thread_count, "overloaded new v7");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
