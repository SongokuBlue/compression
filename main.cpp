#include <iostream>
#include <string>
#include "include/huffman.h"
//
//
//// Hàm tách phần tên file không có phần mở rộng
//std::string getFileNameWithoutExtension(const std::string& filePath) {
//    size_t lastDot = filePath.find_last_of(".");      // Tìm dấu `.` cuối cùng
//    return (lastDot == std::string::npos) ? filePath : filePath.substr(0, lastDot);
//}
//std::string getFileNameWithExtension(const std::string& filePath) {
//    size_t lastDot = filePath.find_last_of(".");      // Tìm dấu `.` cuối cùng
//    return (lastDot == std::string::npos) ? filePath : filePath.substr( lastDot);
//}
//int main(int argc, char* argv[]) {
//    if (argc < 2) {
//        std::cout << "Usage: " << argv[0] << " <file1> <file2> ...\n";
//        return 1;
//    }
//
//    for (int i = 1; i < argc; ++i) {
//        std::string file_name = argv[i];
//        std::string baseName = getFileNameWithoutExtension(file_name);
//        std::string LastName = getFileNameWithExtension(file_name);
//        // Tạo tên file nén & giải nén
//        std::string outputFileName = baseName + ".huff";
//        std::string decodedFileName = baseName + "_decoded" + LastName;
//        std::cout << "Processing file: " << file_name << "\n";
//        MinHeapNode* root = nullptr;
//        compressFile(file_name, outputFileName, root);
//        decompressFile(outputFileName, decodedFileName, root);
//
//        return 0;
//    }
//}

#include <iostream>
#include <string>
#include "huffmanImage.h"
void printHuffmanTree(struct pixfreq *root, int depth) {
    if (root == NULL) {
        return; // Nếu node là NULL, chỉ cần trả về (đừng làm gì cả)
    }
    // In dấu "-" để biểu diễn độ sâu của node
    for (int i = 0; i < depth; i++) {
        printf("-");
    }

    // In thông tin node
    if (root->pix != -1) {
        printf(" (%d, %.6f)\n", root->pix, root->freq);
    } else {
        printf(" (internal, %.6f)\n", root->freq);
    }

    // Đệ quy in cây con trái và phải
    printHuffmanTree(root->left, depth + 1);
    printHuffmanTree(root->right, depth + 1);
}


void verifyImageSize(const char *inputFile) {
    FILE *file = fopen(inputFile, "rb");
    if (!file) {
        printf("Error: Cannot open file\n");
        return;
    }

    int width, height;

    // Đọc kích thước ảnh từ tệp
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);

    fclose(file);

    // Kiểm tra lại kích thước ảnh
    printf("Width: %d, Height: %d\n", width, height);
    // Bạn có thể thay thế printf bằng một cách kiểm tra khác, chẳng hạn lưu vào biến hoặc so sánh với giá trị trước đó
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input BMP file> ..." << std::endl;
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string file_name = argv[i];

        // Tạo tên file output (thay .bmp bằng .huff)
        size_t dotPos = file_name.find_last_of(".");
        std::string baseName = (dotPos == std::string::npos) ? file_name : file_name.substr(0, dotPos);
        std::string outputFileName = baseName + ".huff";
        std::string Image = baseName+"_decoded"+".bmp";

        std::cout << "Compressing: " << file_name << " -> " << outputFileName << std::endl;

        // Khởi tạo dữ liệu hình ảnh
        ImageData data;
        std::unordered_map<int, std::string> huffmanMap;
        readBMP(file_name.c_str(), &data);

        // Tính histogram
        computeHistogram(&data, data.histogram);

        // Xây dựng cây Huffman
        HuffmanTree tree = {0};
        buildHuffmanTree(&data, &tree,huffmanMap);
//        printHuffmanTree(tree.nodes[0], 0);
        // Tạo mã Huffman


        // Mã hóa và lưu file
        encodeImage(outputFileName.c_str(), &data,huffmanMap);


        decodeImage(outputFileName.c_str(),Image.c_str(),&data,huffmanMap);
        verifyImageSize(Image.c_str());
    }


    return 0;
}
