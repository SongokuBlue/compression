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
        }
        else {
            std::string outputFileName = baseName + ".huff";
            std::string decodedFileName = baseName + "_decoded" + LastName;
            std::cout << "Processing text file: " << file_name << "\n";


            MinHeapNode* root = nullptr;
            compressFile(file_name, outputFileName, root);
            decompressFile(outputFileName, decodedFileName, root);
        }
    }

    return 0;
}
