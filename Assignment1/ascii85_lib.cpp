#include "ascii85.hpp"

std::vector<uint8_t> ASCII85::decode(const std::string& input) 
{
    std::vector<uint8_t> result;
    size_t i = 0;
    
    while (i < input.length()) 
    {
        if (std::isspace(input[i])) 
        {
            ++i;
            continue;
        }

        if (input[i] == 'z') 
        {
            result.insert(result.end(), {0, 0, 0, 0});
            ++i;
            continue;
        }

        std::string group;
        size_t j = i;
        while (j < input.length() && group.length() < 5) 
        {
            if (!std::isspace(input[j])) 
            {
                group += input[j];
            }
            ++j;
        }

        if (group.empty()) 
        {
            break;
        }

        if (group.length() == 1) 
        {
            throw std::runtime_error("Invalid input data length");
        }

        uint32_t value = 0;
        for (char c : group) 
        {
            if (c < '!' || c > 'u') 
            {
                throw std::runtime_error("Invalid character in input data");
            }
            value = value * 85 + (c - '!');
        }

        size_t padding = 5 - group.length();
        for (size_t k = 0; k < padding; ++k) 
        {
            value = value * 85 + 84; // Padding with 'u'
        }

        size_t bytes_to_write = 4 - padding;
        for (size_t k = 0; k < bytes_to_write; ++k) 
        {
            result.push_back((value >> (24 - k * 8)) & 0xFF);
        }
        
        i = j;
    }

    return result;
}

std::string ASCII85::encode(const std::vector<uint8_t>& input) 
{
    std::string result;
    size_t i = 0;

    while (i < input.size()) 
    {
        if (i + 4 <= input.size()) 
        {
            uint32_t value = (static_cast<uint32_t>(input[i]) << 24) |
                             (static_cast<uint32_t>(input[i + 1]) << 16) |
                             (static_cast<uint32_t>(input[i + 2]) << 8) |
                             static_cast<uint32_t>(input[i + 3]);
            
            if (value == 0) 
            {
                result += 'z';
            } 
            else 
            {
                char chunk[5];
                for (int j = 0; j < 5; ++j) 
                {
                    chunk[4 - j] = (value % 85) + '!';
                    value /= 85;
                }
                result.append(chunk, 5);
            }
            i += 4;
        } 
        else 
        {
            uint32_t value = 0;
            int remaining = input.size() - i;
            
            for (int j = 0; j < 4; ++j) 
            {
                if (j < remaining) 
                {
                    value |= static_cast<uint32_t>(input[i + j]) << (24 - j * 8);  // Bitwise OR
                }
            }
            
            char chunk[5];
            for (int j = 0; j < 5; ++j) 
            {
                chunk[4 - j] = (value % 85) + '!';
                value /= 85;
            }
            
            result.append(chunk, remaining + 1);
            i += remaining;
        }
    }

    return result;
} 