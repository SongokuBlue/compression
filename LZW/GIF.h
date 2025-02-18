//
// Created by Admin on 1/26/2025.
//

#ifndef LZW_GIF_H
#define LZW_GIF_H
#include <bits/stdc++.h>
#include "filesystem"
class Gif {
public:
    std::vector<int> encoding(const std::vector<unsigned char>& s1);
    std::vector<unsigned char> decoding(std::vector<int> op);
    std::vector<unsigned char> readGIF(const std::string& inputFilename);
    void writeBitsToFile(const std::vector<int>& compressed_gifData, const std::string &outputFilename, std::vector<uint8_t>& bitCounts);
    std::vector<int> readGIFAndCompress (const std::vector<unsigned char>& gifData, const std::string& outputFilename, std::vector<uint8_t>& bitCounts);
    int calculate_bits(int code);
    std::vector<int> convert_bit_to_int(const std::string& inputFilename, const std::vector<uint8_t> &bitCounts);
    std::vector<unsigned char> readGIFAndDecompress(const std::vector<int> compressed_gifData, const std::string& outputFilename);
    void initialize_compress_Dictionary(std::unordered_map<std::string, int>& table);
    void initialize_decompress_Dictionary(std::unordered_map<int, std::string>& table);
    std::vector<unsigned char> get_gifData() const;
    std::vector<int> get_compressed_gifData() const;
    std::vector<unsigned char> get_decompressed_gifData() const;
private:
    std::vector<unsigned char> gifData;
    std::vector<int> compressed_gifData;
    std::vector<unsigned char> decompressed_gifData;
};
#endif //LZW_GIF_H
