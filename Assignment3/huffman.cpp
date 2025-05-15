#include "huffman.h"

std::unordered_map<unsigned char, int> HuffmanEncoder::analyzeFile(const std::string& filePath)
{
    std::unordered_map<unsigned char, int> frequencies;
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file)
    {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    
    unsigned char byte;
    while (file.read(reinterpret_cast<char*>(&byte), 1))
    {
        frequencies[byte]++;
    }
    
    file.close();
    return frequencies;
}

std::shared_ptr<HuffmanNode> HuffmanEncoder::buildHuffmanTree(const std::unordered_map<unsigned char, int>& frequencies)
{
    std::priority_queue<std::shared_ptr<HuffmanNode>, std::vector<std::shared_ptr<HuffmanNode>>, CompareNodes> pq;
    
    for (const auto& pair : frequencies)
    {
        pq.push(std::make_shared<HuffmanNode>(pair.first, pair.second));
    }
    
    if (pq.empty())
    {
        return nullptr;
    }
    if (pq.size() == 1)
    {
        auto leaf = pq.top();
        pq.pop();
        return std::make_shared<HuffmanNode>(0, leaf, nullptr);
    }
    
    while (pq.size() > 1)
    {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        
        auto parent = std::make_shared<HuffmanNode>(left->frequency + right->frequency, left, right);
        
        pq.push(parent);
    }
    
    return pq.top();
}

void HuffmanEncoder::generateCodes(std::shared_ptr<HuffmanNode> node, const std::string& code)
{
    if (!node)
    {
        return;
    }
    
    if (node->isLeaf())
    {
        dictionary.encoding[node->data] = code;
        return;
    }
    
    if (node->left)
    {
        generateCodes(node->left, code + "0");
    }
    if (node->right)
    {
        generateCodes(node->right, code + "1");
    }
}

void HuffmanEncoder::saveDictionary(const std::string& filePath)
{
    std::ofstream file(filePath, std::ios::binary);
    
    if (!file)
    {
        throw std::runtime_error("Failed to open file for writing dictionary: " + filePath);
    }
    
    uint32_t size = dictionary.encoding.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    for (const auto& pair : dictionary.encoding)
    {
        unsigned char byte = pair.first;
        const std::string& code = pair.second;
        
        file.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
        
        uint32_t codeLength = code.length();
        file.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));
        
        file.write(code.c_str(), codeLength);
    }
    
    file.close();
}

void HuffmanEncoder::encodeFile(const std::string& inputFile, const std::string& outputFile, const std::string& dictFile)
{
    auto frequencies = analyzeFile(inputFile);

    root = buildHuffmanTree(frequencies);
    dictionary.root = root;
    
    generateCodes(root, "");
    
    saveDictionary(dictFile);
    
    std::ifstream inFile(inputFile, std::ios::binary);
    std::ofstream outFile(outputFile, std::ios::binary);
    
    if (!inFile || !outFile)
    {
        throw std::runtime_error("Failed to open files for encoding");
    }
    
    unsigned char paddingPlaceholder = 0;
    outFile.write(reinterpret_cast<const char*>(&paddingPlaceholder), sizeof(paddingPlaceholder));
    
    std::string bitBuffer;
    
    unsigned char byte;
    while (inFile.read(reinterpret_cast<char*>(&byte), 1))
    {
        bitBuffer += dictionary.encoding[byte];
        
        while (bitBuffer.length() >= 8)
        {
            unsigned char codedByte = 0;
            for (int i = 0; i < 8; i++)
            {
                if (bitBuffer[i] == '1')
                {
                    codedByte |= (1 << (7 - i));
                }
            }
            outFile.write(reinterpret_cast<const char*>(&codedByte), sizeof(codedByte));
            bitBuffer = bitBuffer.substr(8);
        }
    }
    
    unsigned char padding = 0;
    if (!bitBuffer.empty())
    {
        padding = 8 - bitBuffer.length();
        bitBuffer.append(padding, '0');
        
        unsigned char codedByte = 0;
        for (int i = 0; i < 8; i++)
        {
            if (bitBuffer[i] == '1')
            {
                codedByte |= (1 << (7 - i));
            }
        }
        outFile.write(reinterpret_cast<const char*>(&codedByte), sizeof(codedByte));
    }
    
    outFile.seekp(0);
    outFile.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
    
    inFile.close();
    outFile.close();
}

void HuffmanDecoder::loadDictionary(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file)
    {
        throw std::runtime_error("Failed to open dictionary file: " + filePath);
    }
    
    uint32_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    std::unordered_map<unsigned char, std::string> encodingMap;
    
    for (uint32_t i = 0; i < size; ++i)
    {
        unsigned char byte;
        uint32_t codeLength;
        
        file.read(reinterpret_cast<char*>(&byte), sizeof(byte)); 
        file.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
        
        std::string code(codeLength, ' ');
        file.read(&code[0], codeLength);

        encodingMap[byte] = code;
    }
    
    file.close();
    
    dictionary.encoding = encodingMap;
    dictionary.root = std::make_shared<HuffmanNode>(0, 0); // Dummy root
    
    for (const auto& pair : encodingMap)
    {
        unsigned char symbol = pair.first;
        const std::string& code = pair.second;
        
        std::shared_ptr<HuffmanNode> currentNode = dictionary.root;
        
        for (size_t i = 0; i < code.length(); ++i)
        {
            if (code[i] == '0')
            {
                if (!currentNode->left)
                {
                    currentNode->left = std::make_shared<HuffmanNode>(0, 0);
                }
                currentNode = currentNode->left;
            } else
            {
                if (!currentNode->right)
                {
                    currentNode->right = std::make_shared<HuffmanNode>(0, 0);
                }
                currentNode = currentNode->right;
            }
            
            if (i == code.length() - 1)
            {
                currentNode->data = symbol;
            }
        }
    }
}

void HuffmanDecoder::decodeFile(const std::string& inputFile, const std::string& outputFile, const std::string& dictFile) 
{
    loadDictionary(dictFile);

    std::ifstream inFile(inputFile, std::ios::binary);
    std::ofstream outFile(outputFile, std::ios::binary);
    
    if (!inFile || !outFile)
    {
        throw std::runtime_error("Failed to open files for decoding");
    }
    
    inFile.seekg(0, std::ios::end);
    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    
    if (fileSize <= static_cast<std::streamsize>(sizeof(unsigned char)))
    {
        inFile.close();
        outFile.close();
        return;
    }
    
    unsigned char padding;
    inFile.read(reinterpret_cast<char*>(&padding), sizeof(padding));
    
    std::shared_ptr<HuffmanNode> currentNode = dictionary.root;
    
    if (!currentNode)
    {
        inFile.close();
        outFile.close();
        return;
    }
    
    std::streamsize dataSize = fileSize - sizeof(padding);
    std::streamsize bytesRead = 0;
    
    unsigned char inByte;
    
    while (inFile.read(reinterpret_cast<char*>(&inByte), 1))
    {
        bytesRead++;
        
        for (int i = 0; i < 8; i++)
        {
            if (bytesRead == dataSize && i >= (8 - padding))
            {
                break;
            }
            
            bool bit = (inByte & (1 << (7 - i))) != 0;
            
            if (bit)
            {
                currentNode = currentNode->right;
            }
            else
            {
                currentNode = currentNode->left;
            }

            if (currentNode->isLeaf())
            {
                outFile.write(reinterpret_cast<const char*>(&currentNode->data), sizeof(currentNode->data));
                currentNode = dictionary.root;
            }
        }
    }
    
    inFile.close();
    outFile.close();
} 