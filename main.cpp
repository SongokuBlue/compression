#include <iostream>
#include <string>
#include "include/huffman.h"
#include "huffmanImage.h"



std::string getFileNameWithoutExtension(const std::string& filePath) {
    size_t lastDot = filePath.find_last_of(".");
    return (lastDot == std::string::npos) ? filePath : filePath.substr(0, lastDot);
}
std::string getFileNameWithExtension(const std::string& filePath) {
    size_t lastDot = filePath.find_last_of(".");
    return (lastDot == std::string::npos) ? filePath : filePath.substr( lastDot);
}
long long getFileSize(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        return file.tellg();  // return the file size
    } else {
        std::cerr << "Could not open file: " << filePath << std::endl;
        return -1;
    }
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <file1> <file2> ...\n";
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string file_name = argv[i];
        std::string baseName = getFileNameWithoutExtension(file_name);
        std::string LastName = getFileNameWithExtension(file_name);

        // checking file type before proccess
        if (LastName == ".bmp") {
            // Tạo tên file output (thay .bmp bằng .huff)
            std::string outputFileName = baseName + ".huff";
            std::string Image = baseName + "_decoded" + ".bmp";

            std::cout << "Compressing: " << file_name << " -> " << outputFileName << std::endl;


            ImageData data;
            std::unordered_map<int, std::string> huffmanMap;
            readBMP(file_name.c_str(), &data);


            computeHistogram(&data, data.histogram);


            HuffmanTree tree = {0};
            buildHuffmanTree(&data, &tree, huffmanMap);


            encodeImage(outputFileName.c_str(), &data, huffmanMap);
            std::unordered_map<std::string, int> reverseHuffmanMap;
            for (const auto& pair : huffmanMap) {
                reverseHuffmanMap[pair.second] = pair.first;
            }
            decodeImage(outputFileName.c_str(), Image.c_str(), &data, reverseHuffmanMap);
            long long originalSize = getFileSize(file_name);
            long long compressedSize = getFileSize(outputFileName);

            if (originalSize != -1 && compressedSize != -1) {
                std::cout << "Original file size: " << originalSize << " bytes\n";
                std::cout << "Compressed file size: " << compressedSize << " bytes\n";
                std::cout << "Compression ratio: "
                          << static_cast<double>(originalSize) / compressedSize << "\n";
            }
        }
        else {
            std::string outputFileName = baseName + ".huff";
            std::string decodedFileName = baseName + "_decoded" + LastName;
            std::cout << "Processing text file: " << file_name << "\n";


            MinHeapNode* root = nullptr;
            compressFile(file_name, outputFileName, root);
            decompressFile(outputFileName, decodedFileName, root);
            long long originalSize = getFileSize(file_name);
            long long compressedSize = getFileSize(outputFileName);

            if (originalSize != -1 && compressedSize != -1) {
                std::cout << "Original file size: " << originalSize << " bytes\n";
                std::cout << "Compressed file size: " << compressedSize << " bytes\n";
                std::cout << "Compression ratio: "
                          << static_cast<double>(originalSize) / compressedSize << "\n";

            }
        }
        printf("\n");
    }

    return 0;
}
