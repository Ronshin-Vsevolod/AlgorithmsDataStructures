#include <iostream>
#include <string>
#include <filesystem>
#include <stdexcept>
#include "huffman.h"

int main(int argc, char* argv[]) 
{
    try 
    {
        if (argc < 3) 
        {
            std::cerr << "Error: not enough arguments\n";
            return 1;
        }

        std::string command = argv[1];
        std::string inputFile = argv[2];
        std::string outputFile = "output_file";
        std::string dictFile = "dictionary";

        if (command == "encode") 
        {
            std::cout << "Encoding file " << inputFile << " to " << outputFile << "...\n";
            
            HuffmanEncoder encoder;
            encoder.encodeFile(inputFile, outputFile, dictFile);
            
            std::uintmax_t originalSize = std::filesystem::file_size(inputFile);
            std::uintmax_t compressedSize = std::filesystem::file_size(outputFile);
            std::uintmax_t dictSize = std::filesystem::file_size(dictFile);
            std::uintmax_t totalSize = compressedSize + dictSize;
            
            double compressionRatio = originalSize > 0 
                ? (1.0 - static_cast<double>(totalSize) / originalSize) * 100.0 
                : 0.0;
            
            std::cout << "Encoding completed.\n";
            std::cout << "Original size: " << originalSize << " bytes\n";
            std::cout << "Compressed file size: " << compressedSize << " bytes\n";
            std::cout << "Dictionary size: " << dictSize << " bytes\n";
            std::cout << "Total size: " << totalSize << " bytes\n";
            std::cout << "Compression ratio: " << compressionRatio << "%\n";
        }
        else if (command == "decode") 
        {
            std::cout << "Decoding file " << inputFile << " to " << outputFile << "...\n";
            
            HuffmanDecoder decoder;
            decoder.decodeFile(inputFile, outputFile, dictFile);
            
            std::cout << "Decoding completed.\n";
        }
        else 
        {
            std::cerr << "Error: unknown command: " << command << "\n";
            return 1;
        }
        
        return 0;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 