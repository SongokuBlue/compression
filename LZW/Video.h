//
// Created by Admin on 1/29/2025.
//

#ifndef LZW_VIDEO_H
#define LZW_VIDEO_H
#include <bits/stdc++.h>
class Video {
public:

    std::vector<unsigned char> readVideo(const std::string& inputFilename); // read Video data
    std::vector<int> encoding(const std::vector<unsigned char>& s1); // converting Video data into int vector
    void initialize_compress_Dictionary(std::unordered_map<std::string, int>& table);
    // Compress file
    std::vector<int> readVideoAndCompress (const std::vector<unsigned char>& videoData, const std::string& outputFilename, std::vector<uint8_t>& bitCounts);
    int calculate_bits(int code); // calculate exactly bit for each element in vector<int>
    // Converting vector<int> to binary data
    void writeBitsToFile(const std::vector<int>& compressed_videoData, const std::string &outputFilename, std::vector<uint8_t>& bitCounts);
    // converting binary data back to int data
    std::vector<int> convert_bit_to_int(const std::string& inputFilename, const std::vector<uint8_t> &bitCounts);
    //converting int back to original data
    std::vector<unsigned char> decoding(std::vector<int> op);
    void initialize_decompress_Dictionary(std::unordered_map<int, std::string>& table);
    std::vector<unsigned char> readVideoAndDecompress(const std::vector<int> compressed_videoData, const std::string& outputFilename);
    std::vector<unsigned char> get_videoData() const;
    std::vector<int> get_compressed_videoData() const;
    std::vector<unsigned char> get_decompressed_videoData() const;
private:
    std::vector<unsigned char> videoData;
    std::vector<int> compressed_videoData;
    std::vector<unsigned char> decompressed_videoData;
};
#endif //LZW_VIDEO_H
