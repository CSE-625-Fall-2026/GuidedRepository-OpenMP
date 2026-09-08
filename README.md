# GuidedRepository-OpenMP

This guided repository introduces OpenMP one version at a time. Version 1 is
the serial baseline and does not use OpenMP.

The program creates 200 random `Matrix<double>` objects with MatrixClassDemo
and multiplies them in order. `regular_new` uses the standard allocator.
`overloaded_new` uses CustomMemoryAllocator version 7.

## Principle

Version 1 establishes the serial work that later versions parallelize.

```text
product = matrices[0]
for each remaining matrix
    product = product * matrix
```

## Build and test

The default build type is Debug, so the examples run without optimization.

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run

Both examples use 520 by 520 matrices by default.

```sh
./build/example/regular_new
./build/example/overloaded_new
./build/example/regular_new 300
./build/example/overloaded_new 300
```

# Change Log

- v1.0.0 adds serial multiplication of 200 random matrices
