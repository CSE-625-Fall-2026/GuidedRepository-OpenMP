# GuidedRepository-OpenMP

This guided repository introduces OpenMP one version at a time. Version 5
assigns rolling rows statically to one OpenMP team.

The program creates 200 random `Matrix<double>` objects with MatrixClassDemo
and multiplies them in order. Each worker carries a result row through all 200
matrices. MatrixClassDemo is not changed.

## Principle

Version 5 uses static cyclic scheduling. Worker `i` receives rows `i`,
`i + P`, `i + 2P`, and so on. No dynamic work queue is needed.

```text
create one OpenMP team
statically assign every Pth row to each worker
for each assigned row
    current row = row from matrices[0]
    for each remaining matrix
        calculate the next row with dot products
        swap the current and next row buffers
    store the completed row
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

- v5.0.0 replaces the dynamic row queue with static cyclic scheduling
- v4.0.0 uses one OpenMP team with dynamically scheduled rolling rows
- v3.0.0 divides every result matrix into rows with an OpenMP parallel loop
- v2.0.0 divides the matrix array into chunks with an OpenMP parallel loop
- v1.0.0 adds serial multiplication of 200 random matrices
