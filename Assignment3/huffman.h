#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <memory>

struct HuffmanNode
{
    unsigned char data;
    int frequency;
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;
    
    HuffmanNode(unsigned char data, int frequency) 
        : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
    
    HuffmanNode(int frequency, std::shared_ptr<HuffmanNode> left, std::shared_ptr<HuffmanNode> right)
        : data(0), frequency(frequency), left(left), right(right) {}
    
    bool isLeaf() const 
    {
        return left == nullptr && right == nullptr;
    }
};

struct CompareNodes
{
    bool operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b)
    {
        return a->frequency > b->frequency;
    }
};

struct Dictionary
{
    std::unordered_map<unsigned char, std::string> encoding;
    std::shared_ptr<HuffmanNode> root;
};

class HuffmanEncoder
{
private:
    Dictionary dictionary;
    std::shared_ptr<HuffmanNode> root;

    std::unordered_map<unsigned char, int> analyzeFile(const std::string& filePath);
    std::shared_ptr<HuffmanNode> buildHuffmanTree(const std::unordered_map<unsigned char, int>& frequencies);
    void generateCodes(std::shared_ptr<HuffmanNode> node, const std::string& code);
    void saveDictionary(const std::string& filePath);

public:
    HuffmanEncoder() = default;
    ~HuffmanEncoder() = default;

    void encodeFile(const std::string& inputFile, const std::string& outputFile, const std::string& dictFile);

    const Dictionary& getDictionary() const 
    {
        return dictionary;
    }
};

class HuffmanDecoder
{
private:
    Dictionary dictionary;

    void loadDictionary(const std::string& filePath);

public:
    HuffmanDecoder() = default;
    ~HuffmanDecoder() = default;

    void decodeFile(const std::string& inputFile, const std::string& outputFile, const std::string& dictFile);

    const Dictionary& getDictionary() const 
    {
        return dictionary;
    }
};

#endif