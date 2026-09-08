#include "Example.hpp"

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
        return example::run(matrix_size, "regular new");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
