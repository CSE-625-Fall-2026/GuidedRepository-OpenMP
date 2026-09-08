# GuidedRepository-OpenMP

This guided repository introduces OpenMP one version at a time. Version 3 uses
every OpenMP worker for every matrix multiplication.

The program creates 200 random `Matrix<double>` objects with MatrixClassDemo
and multiplies them in order. Workers calculate separate result rows through
MatrixClassDemo `get()` and `set()`. MatrixClassDemo is not changed.

## Principle

Version 3 applies a parallel `for` to the rows of each result matrix. Its
implicit barrier completes one product before the next product begins.

```text
product = matrices[0]
for each remaining matrix
    create an empty result matrix
    parallel for each result row
        for each column
            calculate the dot product with get()
            store the value with set()
    product = result
```

## OpenMP setup

macOS: install `libomp` with `brew install libomp` and configure with
`-DOpenMP_ROOT="$(brew --prefix libomp)"`.

Linux: install `g++ libomp-dev` with `apt`, or `gcc-c++ libgomp` with `dnf`.

## Build and test

The default build type is Debug, so the examples run without optimization.

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run

Both examples require the matrix size and OpenMP thread count. The thread
count cannot exceed the matrix size.

```sh
./build/example/regular_new 520 8
./build/example/overloaded_new 520 8
```

# Change Log

- v3.0.0 divides every result matrix into rows with an OpenMP parallel loop
- v2.0.0 divides the matrix array into chunks with an OpenMP parallel loop
- v1.0.0 adds serial multiplication of 200 random matrices
