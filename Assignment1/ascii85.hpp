#ifndef ASCII85_HPP
#define ASCII85_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <cstdint>

class ASCII85 
{
public:
    static std::vector<uint8_t> decode(const std::string& input);
    static std::string encode(const std::vector<uint8_t>& input);
};

#endif