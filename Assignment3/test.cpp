#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <random>
#include "huffman.h"

namespace fs = std::filesystem;

std::string calculateMD5(const std::string& filePath)
{
    std::string command = "md5sum " + filePath + " | cut -d' ' -f1";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        return "ERROR";
    }
    
    char buffer[33];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result = buffer;
        if (!result.empty() && result.back() == '\n')
        {
            result.pop_back();
        }
    }
    
    pclose(pipe);
    return result;
}

class HuffmanTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        testDir = fs::temp_directory_path() / "huffman_test";
        if (fs::exists(testDir))
        {
            fs::remove_all(testDir);
        }
        fs::create_directories(testDir);
    }
    
    void TearDown() override
    {
        if (fs::exists(testDir))
        {
            fs::remove_all(testDir);
        }
    }
    
    bool encodeAndDecode(const std::string& inputFile)
    {
        std::string encodedFile = "output_file";
        std::string decodedFile = "output_file_decoded";
        std::string dictFile = "dictionary";
        
        std::string originalMD5 = calculateMD5(inputFile);
        
        HuffmanEncoder encoder;
        encoder.encodeFile(inputFile, encodedFile, dictFile);
        
        HuffmanDecoder decoder;
        decoder.decodeFile(encodedFile, decodedFile, dictFile);
        
        std::string decodedMD5 = calculateMD5(decodedFile);
        
        if (fs::exists(encodedFile)) fs::remove(encodedFile);
        if (fs::exists(decodedFile)) fs::remove(decodedFile);
        if (fs::exists(dictFile)) fs::remove(dictFile);
        
        return originalMD5 == decodedMD5;
    }
    
    fs::path testDir;
};

// Test for an empty file
TEST_F(HuffmanTest, EmptyFile)
{
    std::string emptyFile = (testDir / "empty.txt").string();
    std::ofstream(emptyFile).close();
    
    EXPECT_TRUE(encodeAndDecode(emptyFile));
}

// Test for a file with a single character
TEST_F(HuffmanTest, SingleCharacter)
{
    std::string singleCharFile = (testDir / "single_char.txt").string();
    std::ofstream file(singleCharFile);
    file << "A";
    file.close();
    
    EXPECT_TRUE(encodeAndDecode(singleCharFile));
}

// Test for a file with repeating characters
TEST_F(HuffmanTest, RepeatingCharacters)
{
    std::string repeatingFile = (testDir / "repeating.txt").string();
    std::ofstream file(repeatingFile);
    for (int i = 0; i < 1000; i++)
    {
        file << "A";
    }
    file.close();
    
    EXPECT_TRUE(encodeAndDecode(repeatingFile));
}

// Test for a file with different character frequencies
TEST_F(HuffmanTest, VariableFrequencies)
{
    std::string varFreqFile = (testDir / "var_freq.txt").string();
    std::ofstream file(varFreqFile);
    
    for (int i = 0; i < 50; i++) file << "A";
    for (int i = 0; i < 30; i++) file << "B";
    for (int i = 0; i < 20; i++) file << "C";
    for (int i = 0; i < 10; i++) file << "D";
    file.close();
    
    EXPECT_TRUE(encodeAndDecode(varFreqFile));
}

// Test for a text file with Russian text
TEST_F(HuffmanTest, RussianText)
{
    std::string textFile = (testDir / "russian.txt").string();
    std::ofstream file(textFile);
    file << "Пришел сон из семи сёл.\n";
    file << "Пришла лень из семи деревень.\n";
    file << "Собирались лечь, да простыла печь.\n";
    file << "Окна смотрят на север.\n";
    file << "Сторожит у ручья скирда ничья,\n";
    file << "и большак развезло, хоть бери весло.\n";
    file << "Уронил подсолнух башку на стебель.";
    file.close();
    
    EXPECT_TRUE(encodeAndDecode(textFile));
}

// Test for binary data
TEST_F(HuffmanTest, BinaryData)
{
    std::string binaryFile = (testDir / "binary.dat").string();
    std::ofstream file(binaryFile, std::ios::binary);
    
    for (int i = 0; i < 256; i++)
    {
        char c = static_cast<char>(i);
        file.write(&c, 1);
    }
    file.close();
    
    EXPECT_TRUE(encodeAndDecode(binaryFile));
}

// Test for a very large file
TEST_F(HuffmanTest, LargeFile)
{
    std::string largeFile = (testDir / "large.dat").string();
    std::ofstream file(largeFile, std::ios::binary);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    const int size = 5 * 1024 * 1024;
    for (int i = 0; i < size; i++)
    {
        char c = static_cast<char>(dis(gen));
        file.write(&c, 1);
    }
    file.close();
    
    EXPECT_TRUE(encodeAndDecode(largeFile));
}

// Compression ratio test
TEST_F(HuffmanTest, CompressionRatio)
{
    std::string testFile = (testDir / "compression_test.txt").string();
    std::ofstream file(testFile);
    
    for (int i = 0; i < 10000; i++)
    {
        file << "AAAAAAAAAA";
    }
    file.close();
    
    std::string encodedFile = "output_file";
    std::string dictFile = "dictionary";
    
    HuffmanEncoder encoder;
    encoder.encodeFile(testFile, encodedFile, dictFile);
    
    std::uintmax_t originalSize = fs::file_size(testFile);
    std::uintmax_t compressedSize = fs::file_size(encodedFile);
    std::uintmax_t dictSize = fs::file_size(dictFile);
    std::uintmax_t totalSize = compressedSize + dictSize;
    
    double compressionRatio = (1.0 - static_cast<double>(totalSize) / originalSize) * 100.0;
    EXPECT_GT(compressionRatio, 50.0);
    
    if (fs::exists(encodedFile)) fs::remove(encodedFile);
    if (fs::exists(dictFile)) fs::remove(dictFile);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 