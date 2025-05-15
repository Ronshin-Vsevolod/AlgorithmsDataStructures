# Coder & decoder with Huffman algorithm

## Author

Vsevolod Ronshin, Group 81

## Contacts

st132572@student.spbu.ru

## Description

This project implements the Huffman algorithm for data compression and recovery.

The Huffman algorithm works as follows:
1. analyzes the input data and calculates the frequency of each character
2. Constructs a binary Huffman tree based on the frequencies of the symbols
3. Generates prefix codes for each character, where more frequent characters get shorter codes
4. Encodes the data using the obtained codes

Implementation Features:
- Huffman tree is implemented using std::shared_ptr for memory management
- The encoding dictionary is stored in memory as std::unordered_map, where keys are bytes and values are strings of '0' and '1'
- A constructed Huffman tree is used for decoding
- The binary format is used to store the dictionary in a file

## Build

make

## Run

./huffman encode input_file
./huffman decode output_file

## Testing

./test-huffman

## Cleaning

make clean