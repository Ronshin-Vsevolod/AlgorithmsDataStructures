#include "ascii85.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>

void printUsage() 
{
    std::cerr << "Usage: ascii85 [-e|-d]\n"
              << "  -e: encode (default)\n"
              << "  -d: decode\n";
}

int main(int argc, char* argv[]) 
{
    bool decode_mode = false;

    if (argc > 2) 
    {
        printUsage();
        return 1;
    }

    if (argc == 2) 
    {
        if (strcmp(argv[1], "-d") == 0) 
        {
            decode_mode = true;
        } 
        else if (strcmp(argv[1], "-e") != 0) 
        {
            printUsage();
            return 1;
        }
    }

    try 
    {
        if (decode_mode) 
        {
            std::string input;
            char c;
            
            while (std::cin.get(c)) 
            {
                input.push_back(c);
            }
            
            std::vector<uint8_t> result = ASCII85::decode(input);
            
            std::cout.write(reinterpret_cast<const char*>(result.data()), result.size()); // Slower, but handles null bytes
        } 
        else 
        {
            std::vector<uint8_t> input;
            char c;
            
            while (std::cin.get(c)) // This method seemed simpler, plus now we don't have to worry about buffer length
            {
                input.push_back(static_cast<uint8_t>(c));
            }
            
            std::string result = ASCII85::encode(input);
            
            std::cout << result << std::endl;
        }
        return 0;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 