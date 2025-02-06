#include <iostream>
#include <string>
#include "include/huffman.h"
#include "include/huffman_image.h"
int main(int argc, char* argv[]) {
    if (argc < 2) { // Kiểm tra xem có đủ tham số không
        std::cerr << "Usage: huffman <input_file1> [<input_file2> ...]\n";
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) { // Lặp qua các tệp được truyền từ dòng lệnh
        const std::string file_name = argv[i];
        // Tạo tên file nén
        const std::string outputFileName = "compress.huff";

        std::cout << "Processing file: " << file_name << "\n";

        // Gọi hàm nén
        const std::string output = "encode.bmp";
        MinHeapNodeImage* root = nullptr; // Khởi tạo con trỏ root
        compressImage(file_name, outputFileName, root);  // Truyền tham chiếu vào đây
        decompressImage(outputFileName, output, root);  // Sử dụng con trỏ root đã cập nhật
    }
    return EXIT_SUCCESS;
}
