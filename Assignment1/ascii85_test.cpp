#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ascii85.hpp"

TEST(ASCII85Test, EncodeBasicTest) 
{
    std::vector<uint8_t> input = {'H', 'e', 'l', 'l', 'o'};
    std::string encoded = ASCII85::encode(input);
    EXPECT_EQ(encoded, "87cURDZ");
}

TEST(ASCII85Test, DecodeBasicTest) 
{
    std::string input = "87cURD]";
    std::vector<uint8_t> decoded = ASCII85::decode(input);
    std::vector<uint8_t> expected = {'H', 'e', 'l', 'l', 'o'};
    EXPECT_EQ(decoded, expected);
}

TEST(ASCII85Test, EncodeZeroBlockTest) 
{
    std::vector<uint8_t> input = {0, 0, 0, 0};
    std::string encoded = ASCII85::encode(input);
    EXPECT_EQ(encoded, "z");
}

TEST(ASCII85Test, DecodeZeroBlockTest) 
{
    std::string input = "z";
    std::vector<uint8_t> decoded = ASCII85::decode(input);
    std::vector<uint8_t> expected = {0, 0, 0, 0};
    EXPECT_EQ(decoded, expected);
}

TEST(ASCII85Test, DecodeWithSpacesTest) 
{
    std::string input = "87cUR D]";
    std::vector<uint8_t> decoded = ASCII85::decode(input);
    std::vector<uint8_t> expected = {'H', 'e', 'l', 'l', 'o'};
    EXPECT_EQ(decoded, expected);
}

TEST(ASCII85Test, DecodeInvalidCharacterTest) 
{
    std::string input = "87cUR{}]";
    EXPECT_THROW(ASCII85::decode(input), std::runtime_error);
}

TEST(ASCII85Test, DecodeInvalidLengthTest) 
{
    std::string input = "]";
    EXPECT_THROW(ASCII85::decode(input), std::runtime_error);
}

TEST(ASCII85Test, EncodeLongTextTest) 
{
    std::vector<uint8_t> input = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 
                                 'a', ' ', 'l', 'o', 'n', 'g', ' ', 't', 
                                 'e', 'x', 't', ' ', 'm', 'e', 's', 's',
                                 'a', 'g', 'e', '.'};
    std::string encoded = ASCII85::encode(input);
    std::vector<uint8_t> decoded = ASCII85::decode(encoded);
    EXPECT_EQ(decoded, input);
}

TEST(ASCII85Test, EncodeEmptyTest) 
{
    std::vector<uint8_t> input;
    std::string encoded = ASCII85::encode(input);
    EXPECT_EQ(encoded, "");
}

TEST(ASCII85Test, DecodeEmptyTest) 
{
    std::string input = "";
    std::vector<uint8_t> decoded = ASCII85::decode(input);
    EXPECT_TRUE(decoded.empty());
}

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 