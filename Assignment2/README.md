# Gauss_Task

## Author

Vsevolod Ronshin, Group 81

## Contacts

st132572@student.spbu.ru

## Description

A program for solving systems of linear equations using the Gaussian elimination method. The system (coefficient matrix A and constant vector b) is read from a CSV file. The Eigen library is used for efficient matrix operations.

The program implements the following features:
- Reading the coefficient matrix and constant vector from a CSV file
- Solving linear equation systems using Gaussian elimination with explicit row operations
- Generating large systems using a reproducible pseudorandom number generator
- Outputting the result in CSV format

The project is equipped with unit tests using Google Test.

## Build

```bash
cd build
cmake ..
make
```

## Run

Basic usage (reads from default.csv):
```bash
./Main
```

Using a specific CSV file:
```bash
./Main path/to/file.csv
```

Generating a random system:
```bash
./Main --generate [size] [seed]
```
where:
- `size` is the size of the system (optional, default: 3)
- `seed` is the random seed (optional, default: 42)

## Testing

```bash
./Runner
```

## Cleaning the Project

```bash
cd build
make clean

# to completely remove all build artifacts
make cleanall
```