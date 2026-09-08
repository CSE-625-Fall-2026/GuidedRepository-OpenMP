# GuidedRepository-OpenMP

This guided repository introduces OpenMP one version at a time. Version 2
divides the matrix array into contiguous chunks.

The program creates 200 random `Matrix<double>` objects with MatrixClassDemo
and multiplies them in order. Each OpenMP worker calculates one chunk, then
the caller multiplies the chunk results in order. Both allocation examples are
provided.

## Principle

Version 2 uses a parallel `for` to distribute independent matrix chunks.

```text
q = floor(200 / P)
r = 200 mod P
C_i = q + 1 when i < r, otherwise q
S_i = iq + min(i, r)

parallel for each chunk i
    multiply matrices S_i through S_i + C_i - 1
multiply the chunk results in order
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

Both examples require the matrix size and OpenMP thread count.

```sh
./build/example/regular_new 520 8
./build/example/overloaded_new 520 8
```

# Change Log

- v2.0.0 divides the matrix array into chunks with an OpenMP parallel loop
- v1.0.0 adds serial multiplication of 200 random matrices
