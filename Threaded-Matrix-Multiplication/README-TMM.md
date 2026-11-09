# Threaded Matrix Multiplication

Compares single-threaded and multithreaded matrix multiplication in C using POSIX threads. Both versions print execution time and verify the result with a second calculation.

## Build and run

Requires GCC, Make, and a POSIX system such as Linux.

```bash
make -f Makefile-single-thread
make -f Makefile-multiple-thread

./matrix_single
./matrix_multi 4
```

Replace `4` with the desired number of threads. Run the threaded executable directly; its Makefile's `run` target currently omits this required argument.

## How it works

- **Single-threaded:** one thread calculates every output cell.
- **Multithreaded:** workers share the calculation, each writing to separate output cells.
- **Verification:** both versions recalculate the product in reverse cell order and compare results. This extra work is excluded from the reported time.

## Configuration

Defaults: **3200 × 3200** matrices with random values from **1 to 9**, using about **156 MiB** for matrix storage.

To change the size, edit `ROWS` and `COLS` in both `.c` files, keep them equal, and rebuild. Smaller matrices are useful for quick runs.

## Clean up

```bash
make -f Makefile-single-thread clean
make -f Makefile-multiple-thread clean
```
