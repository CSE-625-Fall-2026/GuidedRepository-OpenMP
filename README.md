# GuidedRepository-OpenMP

This guided repository introduces OpenMP one version at a time. Version 6
improves cache use inside the statically assigned rolling rows.

The program creates 200 random `Matrix<double>` objects with MatrixClassDemo
and multiplies them in order. Each worker carries a result row through all 200
matrices. MatrixClassDemo is not changed.

## Principle

Version 6 keeps static cyclic scheduling and changes the loop order. Each
worker reads a row of the fixed matrix from left to right and updates the
entire output row, improving spatial locality without changing MatrixClassDemo.

```text
create one OpenMP team
statically assign every Pth row to each worker
for each assigned row
    current row = row from matrices[0]
    for each remaining matrix
        clear the next row
        for each current row value
            update every next row column
        swap the current and next row buffers
    store the completed row
```

## OpenMP setup

macOS: install `libomp` with `brew install libomp` and configure with
`-DOpenMP_ROOT="$(brew --prefix libomp)"`.

Linux: install `g++ libomp-dev` with `apt`, or `gcc-c++ libgomp` with `dnf`.

The [official OpenMP documentation](https://www.openmp.org/specifications/)
provides specifications, examples, and quick-reference guides.

## Common OpenMP directives

| Directive | Common use |
|---|---|
| `#pragma omp parallel` | Create a team of threads for a block |
| `#pragma omp parallel for` | Create a team and divide loop iterations |
| `#pragma omp for` | Divide a loop among an existing team |
| `#pragma omp sections` | Run different independent blocks concurrently |
| `#pragma omp single` | Let one thread execute a block |
| `#pragma omp task` | Create work that any available thread can perform |
| `#pragma omp barrier` | Wait until every thread reaches the same point |
| `#pragma omp critical` | Allow one thread at a time into a block |
| `#pragma omp atomic` | Protect one simple memory update |

## Common clauses and runtime controls

| Command | Common use |
|---|---|
| `num_threads(n)` | Request `n` threads for a parallel region |
| `schedule(static)` | Assign loop work before execution |
| `schedule(dynamic, n)` | Assign chunks of `n` iterations as threads finish |
| `private(value)` | Give each thread its own uninitialized value |
| `firstprivate(value)` | Give each thread an initialized copy |
| `shared(value)` | Share one value among the team |
| `collapse(n)` | Combine `n` nested loops for scheduling |
| `reduction(op: value)` | Combine thread-local values safely |
| `omp_set_num_threads(n)` | Set the default number of threads |
| `omp_get_thread_num()` | Get the current thread number |
| `omp_get_num_threads()` | Get the current team size |
| `omp_get_wtime()` | Read a wall-clock timer |
| `OMP_NUM_THREADS` | Set the default thread count through the environment |
| `OMP_SCHEDULE` | Select runtime loop scheduling through the environment |

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

## Profile results

These results use 200 matrices of size 800 by 800, 10 OpenMP threads, and the
Debug build without optimization. Times are measured in seconds on one machine
and will vary by hardware.

| Version | Implementation | Regular new | Overloaded new v7 |
|---|---|---:|---:|
| v2.0.0 | Matrix array chunks | 170.433 | 145.820 |
| v3.0.0 | Row groups for each product | 173.858 | 164.612 |
| v4.0.0 | Persistent team with dynamic rows | 132.249 | 103.791 |
| v5.0.0 | Persistent team with static rows | 133.747 | 106.141 |
| v6.0.0 | Cache-friendly static rows | 92.235 | 94.543 |

# Change Log

- v6.0.0 reorders the inner loops for cache-friendly row access
- v5.0.0 replaces the dynamic row queue with static cyclic scheduling
- v4.0.0 uses one OpenMP team with dynamically scheduled rolling rows
- v3.0.0 divides every result matrix into rows with an OpenMP parallel loop
- v2.0.0 divides the matrix array into chunks with an OpenMP parallel loop
- v1.0.0 adds serial multiplication of 200 random matrices
