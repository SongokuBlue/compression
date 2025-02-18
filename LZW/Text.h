//
// Created by Admin on 1/27/2025.
//

#ifndef LZW_TEXT_H
#define LZW_TEXT_H
#include <bits/stdc++.h>
class Text{
public:
    std::vector<int> encoding(const std::string & s1);
    std::string decoding(std::vector<int> op);
    std::string readTEXT(const std::string& inputFilename);
    std::vector<int> readTextAndCompress(const std::string& textData, const std::string& outputFilename);
    std::string readTextAndDecompress (const std::string& inputFilename, const std::string& outputFilename);
    void initialize_compress_Dictionary(std::unordered_map<std::string, int>& table);
    void initialize_decompress_Dictionary(std::unordered_map<int, std::string>& table);
    std::string get_textData() const;
    std::vector<int> get_compressed_textData() const;
    std::string get_decompressed_textData() const;

private:
    std::string textData;
    std::vector<int> compressed_textData;
    std::string decompressed_textData;
};
#endif //LZW_TEXT_H
