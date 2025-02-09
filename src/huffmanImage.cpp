#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffmanImage.h"
#include "string"
#include "map"
#include "iostream"
#include "queue"
int compare(const void *a, const void *b) {
    const huffcode *x = *(const huffcode **)a;
    const huffcode *y = *(const huffcode **)b;
    return (x->freq > y->freq) - (x->freq < y->freq);
}

void readBMP(const char *filename, struct ImageData *data) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open the file\n");
        exit(1);
    }

    unsigned char header[BMP_HEADER_SIZE];
    fread(header, sizeof(unsigned char), BMP_HEADER_SIZE, file);

    data->width = *(int *)&header[18];
    data->height = *(int *)&header[22];

    if (data->width <= 0 || data->height <= 0) {
        printf("Error: Invalid image size (%d x %d)\n", data->width, data->height);
        fclose(file);
        exit(1);
    }


    data->image = (unsigned char **)malloc(data->height * sizeof(unsigned char *));
    if (data->image == NULL) {
        printf("Memory allocation failed for data->image!\n");
        exit(1);
    }

    for (int i = 0; i < data->height; i++) {
        data->image[i] = (unsigned char *)malloc(data->width * sizeof(unsigned char));
        if (data->image[i] == NULL) {
            printf("Memory allocation failed for row %d!\n", i);
            exit(1);
        }
    }
    fseek(file, BMP_HEADER_SIZE, SEEK_SET);  // Di chuy?n ??n v? tr� b?t ??u c?a d? li?u ?nh
    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            fread(&data->image[i][j], sizeof(unsigned char), 1, file);
        }
    }


    fclose(file);
}

void computeHistogram(struct ImageData *data, float hist[MAX_GRAY_VALUE]) {
    memset(hist, 0, sizeof(float) * MAX_GRAY_VALUE);

    // T�nh to�n histogram
    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            hist[data->image[i][j]]++;
        }
    }


    // T�nh t?n su?t chu?n h�a
    float totalPixels = data->width * data->height;
    for (int i = 0; i < MAX_GRAY_VALUE; i++) {
        hist[i] /= totalPixels;
    }


//    printf("Normalized histogram:\n");
//    for (int i = 0; i < MAX_GRAY_VALUE; i++) {
//        if (hist[i] > 0) {
//            printf("Pixel value %d: %f\n", i, hist[i]);
//        }
//    }
}


void buildHuffmanTree(struct ImageData *data, struct HuffmanTree *tree,std::unordered_map<int, std::string> &huffmanMap) {
    int i;
    float *hist = data->histogram;


    std::map<int, float> freqMap;


    for (i = 0; i < MAX_GRAY_VALUE; i++) {
        if (hist[i] > 0) {
            freqMap[i] = hist[i];
        }
    }



    std::priority_queue<pixfreq*, std::vector<pixfreq*>, ComparePixFreq> pq;


    for (const auto& pair : freqMap) {
        pixfreq* node = (pixfreq*)malloc(sizeof(pixfreq));
        if (node == NULL) {
            perror("Memory allocation failed for pixfreq node!");
            exit(1);
        }
        node->pix = pair.first;
        node->freq = pair.second;
        node->left = NULL;
        node->right = NULL;
        pq.push(node);
    }

    // X�y d?ng c�y Huffman
    while (pq.size() > 1) {
        pixfreq* left = pq.top(); pq.pop();
        pixfreq* right = pq.top(); pq.pop();

        pixfreq* newNode = (pixfreq*)malloc(sizeof(pixfreq));
        if (newNode == NULL) {
            perror("Memory allocation failed for new node!");
            exit(1);
        }
        newNode->pix = -1;
        newNode->freq = left->freq + right->freq;
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
        tree->totalNodes++;
        tree->nodes[tree->totalNodes - 1] = newNode;
    }


    tree->nodes[0] = pq.top();
    pq.pop();


    if (!pq.empty()) {
        fprintf(stderr, "Error: Priority queue should be empty after tree construction.\n");
        exit(1);
    }
    generateCodes(tree->nodes[0], "", huffmanMap);

}




void generateCodes(struct pixfreq *node, std::string code, std::unordered_map<int, std::string> &huffmanMap) {
    if (node == nullptr) return;

    // Nếu là nút lá (pixel thực sự)
    if (node->left == NULL && node->right == NULL) {
        huffmanMap[node->pix] = code;
        return;
    }

    // Đệ quy xuống cây con trái và phải
    generateCodes(node->left, code + "0", huffmanMap);
    generateCodes(node->right, code + "1", huffmanMap);
}

void encodeImage(const char *outputFile, struct ImageData *data, std::unordered_map<int, std::string> &huffmanMap) {
    FILE *file = fopen(outputFile, "wb");
    if (!file) {
        printf("can't open file\n");
        return;
    }

    // Ghi kích thước ảnh

    fwrite(&data->width, sizeof(int), 1, file);
    fwrite(&data->height, sizeof(int), 1, file);

    // Ghi số lượng phần tử của Huffman Map
    int mapSize = huffmanMap.size();
    fwrite(&mapSize, sizeof(int), 1, file);

    // Ghi từng cặp {pixel_value, huffman_code}
    for (const auto &pair : huffmanMap) {
        int pixelValue = pair.first;
        std::string code = pair.second;

        int codeLength = code.size();  // Độ dài chuỗi Huffman
        fwrite(&pixelValue, sizeof(int), 1, file);
        fwrite(&codeLength, sizeof(int), 1, file);
        fwrite(code.c_str(), sizeof(char), codeLength, file);
    }

    // Ghi dữ liệu ảnh Huffman
    unsigned char byte = 0;
    int bitCount = 0;

    for (int i = 0; i < data->height; i++) {
        for (int j = 0; j < data->width; j++) {
            unsigned int pixel = data->image[i][j];
            std::string code = huffmanMap[pixel];

            for (char c : code) {
                byte = byte << 1;
                if (c == '1') byte |= 1;
                bitCount++;

                if (bitCount == 8) {
                    fwrite(&byte, sizeof(unsigned char), 1, file);
                    byte = 0;
                    bitCount = 0;
                }
            }
        }
    }

    // Ghi byte cuối nếu còn dư
    if (bitCount > 0) {
        byte <<= (8 - bitCount); // Shift remaining bits to the left
        fwrite(&byte, sizeof(unsigned char), 1, file);
    }

    fclose(file);
}

//void decodeImage(const char *inputFile,const char *OutputFile ,struct ImageData *data, HuffmanTree *tree) {
//    FILE *file = fopen(inputFile, "rb");
//    if (!file) {
//        printf("Error: Cannot open file\n");
//        return;
//    }
//    FILE *outFile = fopen(OutputFile, "wb");
//    if (!outFile) {
//        printf("Error: Cannot create output file\n");
//        fclose(file);
//        return;
//    }
//    // Đọc kích thước ảnh
//    printf("width %d.height %d",data->width,data->height);
//    fread(&data->width, sizeof(int), 1, file);
//    fread(&data->height, sizeof(int), 1, file);
//
//    // Cấp phát bộ nhớ cho ảnh
//    data->image = new unsigned char *[data->height];
//    for (int i = 0; i < data->height; i++) {
//        data->image[i] = new unsigned char[data->width];
//    }
//
//    // Giải mã dữ liệu ảnh từ bitstream
//    unsigned char byte;
//    int row = 0, col = 0;
//    pixfreq *root = tree->nodes[0];  // Gốc cây Huffman
//    pixfreq *currentNode = root;
//
//    while (fread(&byte, sizeof(unsigned char), 1, file)) {
//        for (int i = 7; i >= 0; i--) {
//            bool bit = (byte >> i) & 1;
//            currentNode = bit ? currentNode->right : currentNode->left;
//
//            // Nếu là nút lá (chứa pixel thực sự)
//            if (!currentNode->left && !currentNode->right) {
//                data->image[row][col] = currentNode->pix;  // Gán giá trị pixel
//                currentNode = root;  // Quay lại gốc cây để giải mã pixel tiếp theo
//                col++;
//
//                if (col == data->width) {
//                    col = 0;
//                    row++;
//                    if (row == data->height) {
//                        fclose(file);
//                        goto WRITE_OUTPUT;
//                    }
//                }
//            }
//        }
//    }
//    WRITE_OUTPUT:
//    // Ghi dữ liệu vào file BMP
//    fwrite(&data->width, sizeof(int), 1, outFile);
//    fwrite(&data->height, sizeof(int), 1, outFile);
//
//    for (int i = 0; i < data->height; i++) {
//        fwrite(data->image[i], sizeof(unsigned char), data->width, outFile );
//    }
//
//    printf("Decoded image saved to %s\n", OutputFile);
//    // Đóng file
//    fclose(outFile);
//}

void decodeImage(const char *inputFile, const char *outputFile, struct ImageData *data, const std::unordered_map<std::string, int> &reverseHuffmanMap) {
    FILE *file = fopen(inputFile, "rb");
    if (!file) {
        printf("Error: Cannot open input file\n");
        return;
    }

    FILE *outFile = fopen(outputFile, "wb");
    if (!outFile) {
        printf("Error: Cannot create output file\n");
        fclose(file);
        return;
    }

    // Đọc kích thước ảnh từ file nén
    if (fread(&data->width, sizeof(int), 1, file) != 1 || fread(&data->height, sizeof(int), 1, file) != 1) {
        printf("Error: Failed to read image dimensions\n");
        fclose(file);
        fclose(outFile);
        return;
    }

    // Kiểm tra giá trị width và height hợp lệ
    if (data->width <= 0 || data->height <= 0) {
        printf("Error: Invalid image size (%d x %d)\n", data->width, data->height);
        fclose(file);
        fclose(outFile);
        return;
    }

    // Cấp phát bộ nhớ cho dữ liệu ảnh
    data->image = (unsigned char **)malloc(data->height * sizeof(unsigned char *));
    if (data->image == NULL) {
        printf("Error: Memory allocation failed for image rows\n");
        fclose(file);
        fclose(outFile);
        return;
    }

    for (int i = 0; i < data->height; ++i) {
        data->image[i] = (unsigned char *)malloc(data->width * sizeof(unsigned char));
        if (data->image[i] == NULL) {
            printf("Error: Memory allocation failed for image row %d\n", i);
            fclose(file);
            fclose(outFile);
            return;
        }
    }

    // Đọc số lượng phần tử của Huffman Map
    int mapSize;
    if (fread(&mapSize, sizeof(int), 1, file) != 1) {
        printf("Error: Failed to read Huffman map size\n");
        fclose(file);
        fclose(outFile);
        return;
    }

    // Đọc từng cặp {pixel_value, huffman_code} từ file nén
    std::unordered_map<std::string, int> huffmanMap;
    for (int i = 0; i < mapSize; ++i) {
        int pixelValue;
        int codeLength;
        if (fread(&pixelValue, sizeof(int), 1, file) != 1 || fread(&codeLength, sizeof(int), 1, file) != 1) {
            printf("Error: Failed to read Huffman map entry\n");
            fclose(file);
            fclose(outFile);
            return;
        }

        char *code = (char *)malloc((codeLength + 1) * sizeof(char));
        if (code == NULL) {
            printf("Error: Memory allocation failed for Huffman code\n");
            fclose(file);
            fclose(outFile);
            return;
        }

        if (fread(code, sizeof(char), codeLength, file) != codeLength) {
            printf("Error: Failed to read Huffman code\n");
            free(code);
            fclose(file);
            fclose(outFile);
            return;
        }

        code[codeLength] = '\0'; // Kết thúc chuỗi
        huffmanMap[code] = pixelValue;
        free(code);
    }

    // Giải mã bitstream từ file nén
    unsigned char byte;
    std::string currentBits = "";
    int row = 0, col = 0;

    while (fread(&byte, sizeof(unsigned char), 1, file)) {
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            currentBits += (bit ? "1" : "0");

            // Nếu tìm thấy chuỗi mã Huffman khớp với currentBits
            if (huffmanMap.find(currentBits) != huffmanMap.end()) {
                data->image[row][col] = huffmanMap.at(currentBits);
                currentBits = "";  // Reset chuỗi bit hiện tại

                col++;
                if (col == data->width) {
                    col = 0;
                    row++;
                    if (row == data->height) {
                        fclose(file);
                        goto WRITE_OUTPUT;
                    }
                }
            }
        }
    }

    fclose(file);

    WRITE_OUTPUT:
    // Tạo header BMP
    unsigned char bmpHeader[BMP_HEADER_SIZE] = {0};

    // Điền header BMP
    bmpHeader[0] = 'B';
    bmpHeader[1] = 'M';

    int rowSize = (data->width + 3) & ~3;  // Đảm bảo width là bội số của 4
    int imageSize = rowSize * data->height;
    int fileSize = BMP_HEADER_SIZE + COLOR_TABLE_SIZE + imageSize;
    int offset = BMP_HEADER_SIZE + COLOR_TABLE_SIZE;
    int colorCount = 256;  // 8-bit grayscale có 256 màu

    // Ghi kích thước file và thông tin BMP vào header
    memcpy(&bmpHeader[2], &fileSize, 4);
    memcpy(&bmpHeader[10], &offset, 4);
    memcpy(&bmpHeader[14], "\x28\x00\x00\x00", 4); // DIB Header Size (40 bytes)
    memcpy(&bmpHeader[18], &data->width, 4);
    memcpy(&bmpHeader[22], &data->height, 4);
    memcpy(&bmpHeader[26], "\x01\x00", 2); // Planes = 1
    memcpy(&bmpHeader[28], "\x08\x00", 2); // Bits per pixel = 8
    memcpy(&bmpHeader[34], &imageSize, 4);
    memcpy(&bmpHeader[46], &colorCount, 4);

    fwrite(bmpHeader, sizeof(unsigned char), BMP_HEADER_SIZE, outFile);

    unsigned char colorTable[COLOR_TABLE_SIZE];
    for (int i = 0; i < 256; i++) {
        colorTable[i * 4] = i;
        colorTable[i * 4 + 1] = i;
        colorTable[i * 4 + 2] = i;
        colorTable[i * 4 + 3] = 0;
    }
    fwrite(colorTable, sizeof(unsigned char), COLOR_TABLE_SIZE, outFile);

    // Ghi dữ liệu pixel (thêm padding)
    unsigned char paddingBytes[3] = {0, 0, 0};
    int padding = rowSize - data->width;

    for (int i = data->height - 1; i >= 0; --i) {
        fwrite(data->image[i], sizeof(unsigned char), data->width, outFile);
        fwrite(paddingBytes, sizeof(unsigned char), padding, outFile);
    }

    printf("✅ Decoded image saved to %s\n", outputFile);

    fclose(outFile);
}
