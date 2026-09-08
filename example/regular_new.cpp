#include "Example.hpp"

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
        return example::run(matrix_size, thread_count, "regular new");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
