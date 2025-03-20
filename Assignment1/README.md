# ASCII85 Encoder/Decoder

## Description

Implementation of a program for encoding and decoding data in ASCII85 format.

The program supports:
- Encoding arbitrary binary data to ASCII85
- Decoding ASCII85 back to binary data
- Processing special characters (such as 'z' for blocks of zeros)
- Ignoring whitespace characters during decoding
- Validating input data correctness

## Building

To build the project, you need:
- CMake (version 3.10 or higher)
- C++ compiler with C++17 support
- Internet access (for downloading GoogleTest)

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

The program works with data through standard input/output:

```bash
# Encoding (default)
echo "Hello, World!" | ./ascii85

# Explicit encoding mode
echo "Hello, World!" | ./ascii85 -e

# Decoding
echo "87cURD_*#4DfTZ)+US" | ./ascii85 -d
```

## Testing

The project includes several types of tests:

### Unit tests with GoogleTest

After building, you can run the unit tests:

```bash
./ascii85_test
```

These tests check:
- Encoding and decoding of basic strings
- Processing zero blocks using the 'z' character
- Processing whitespace characters during decoding
- Validation of incorrect input data
- Encoding and decoding of empty data

### Tests using Python

The project also implements tests using Python, which can be run:

```bash
cp build/ascii85 .
python3 python_test.py
```

These tests:
- Compare encoding results with the implementation in Python's `base64` module
- Check encoding and decoding on random data of different sizes
- Test handling of incorrect input data

## Implementation Features

- Encoding: 4 input bytes are converted to 5 ASCII characters
- Special processing for zero blocks (character 'z')
- Decoding allows processing incomplete blocks
- Ignoring whitespace characters during decoding
- Validating input data correctness during decoding
- Handling edge cases (empty input, incomplete blocks) 

## Cleaning the Project

```bash
cd build
make clean

rm -rf build/*