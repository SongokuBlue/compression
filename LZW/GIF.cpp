#include "GIF.h"

std::vector<int> Gif::encoding(const std::vector<unsigned char> &s1) {
    std::cout << "Encoding\n";
    std::unordered_map<std::string, int> table;
    initialize_compress_Dictionary(table);

    std::string p = "", c = ""; // p: current string, c: is a next character
    p += s1[0];  // initialize the first string by plus with first character
    int code = 256; // code number (after maximum ASCII number)
    std::vector<int> output_code; // vector which stored output
//    cout << "String\tOutput_Code\tAddition\n";
    // Iterating each character through string
    for (int i = 0; i < s1.size(); i++) {
        if (i != s1.size() - 1) // if still not reaching the last character
            c = s1[i + 1]; // move to the next character
        // if p+c already exist in dictionary, make p+c be a current pattern
        if (table.find(p + c) != table.end()) {
            p = p + c;
        }
            // if p+c is currently not exist in dictionary, adding a current pattern (p)
        else {
//            cout << p << "\t" << table[p] << "\t\t"   command this code
//                 << p + c << "\t" << code << endl;
            output_code.push_back(table[p]);
            table[p + c] = code; // adding p+c pattern with corresponding code number to dictionary
            code++; // increase code number by 1
            if(code >= 4096) {
                initialize_compress_Dictionary(table); // reset dictionary
                code = 256;
            }
            p = c; // setting current pattern be c
        }
        c = ""; // reset c before checking the next character
    }
//    cout << p << "\t" << table[p] << endl; command this line
    output_code.push_back(table[p]); // push the last pattern to output
    return output_code;
}

std::vector<unsigned char> Gif::decoding(std::vector<int> op) {
    std::cout << "\nDecoding\n";
    std::unordered_map<int, std::string> table;
    //initialize dictionary with ASCII (0-255)
    initialize_decompress_Dictionary(table);

    std::vector<unsigned char> decompressedData;
    int old = op[0]; // get the first code number in encode output
    std::string s = table[old]; // searching dictionary to get first string (always start with character)
    std::string c ;
    c = s[0]; // c is a first character of a current pattern
    int count = 256; // new code number start with 256

    for (char ch : s) {
        decompressedData.push_back(ch); // Add first decoded string to result
    }
    // Iterating each code number in encode output
    for (int i = 0; i < op.size() - 1; i++) {
        int n = op[i + 1]; // next number in output
        // if current code number does not exist in dict
        if (table.find(n) == table.end()) {
            s = table[old]; // get string from previous code number
            s = s + c; // and plus with first character to create a new pattern
        }
        else { // if code number already exist
            s = table[n]; // getting a current string
        }

        for (char ch : s) {
            decompressedData.push_back(ch); // Add decoded string to result
        }
        c = ""; // reset c before set it to be a first character in a next pattern
        c += s[0]; // get the first character of current pattern
        table[count] = table[old] + c; // adding new string to dictionary
        count++; // increase code number by 1
        if (count >= 4096) { // when code number over 12 bits, reset the dictionary
            initialize_decompress_Dictionary(table);
            count = 256;
        }
        old = n; // update old to become previous code number
    }
    return decompressedData;
}

std::vector<unsigned char> Gif::readGIF(const std::string &inputFilename) {
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Opening file " << inputFilename << " failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<unsigned char> gif_Data((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    gifData = gif_Data;
    inputFile.close();

    return gifData;
}



std::vector<int> Gif::readGIFAndCompress(const std::vector<unsigned char>& gifData, const std::string& outputFilename, std::vector<uint8_t>& bitCounts) {
    compressed_gifData = encoding(gifData);
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if(!outputFile.is_open()) {
        std::cerr << "Opening file " << outputFilename << "failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    writeBitsToFile(compressed_gifData, outputFilename, bitCounts);
    return compressed_gifData;
}

std::vector<unsigned char> Gif::readGIFAndDecompress(const std::vector<int> compressed_gifData, const std::string &outputFilename) {
    decompressed_gifData = decoding(compressed_gifData);
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Opening file " << outputFilename << " failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    outputFile.write(reinterpret_cast<const char*>(decompressed_gifData.data()), decompressed_gifData.size());
    outputFile.close();

    std::cout << "Decompressed GIF written to " << outputFilename << " successfully." << std::endl;
    return decompressed_gifData;
}

int Gif::calculate_bits(int code) {
    return (code == 0) ? 1 : (int)log2(code + 1) + 1;
}

void Gif::writeBitsToFile(const std::vector<int>& compressed_gifData, const std::string &outputFilename, std::vector<uint8_t>& bitCounts) {
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Opening file " << outputFilename << " failed!" << std::endl;
        exit(EXIT_FAILURE);
    }


    uint8_t bitBuffer = 0;
    int bitPosition = 0;     //  Current bit position in buffer

    for (int code : compressed_gifData) {
        int bitCount = calculate_bits(code);
        bitCounts.push_back(bitCount);
        for (int i = 0; i < bitCount; ++i) {
            int bit = (code >> (bitCount - 1 - i)) & 1;  // Lấy bit từ trái qua phải
            bitBuffer |= (bit << (7 - bitPosition));  // Chèn bit vào buffer
            bitPosition++;

            if (bitPosition == 8) {  // Khi đủ 8 bit, ghi vào file
                outputFile.put(bitBuffer);
                bitBuffer = 0;  // Reset buffer
                bitPosition = 0; // Reset vị trí bit
            }
        }

    }

    // Ghi nốt byte cuối cùng nếu có dư bit
    if (bitPosition > 0) {
        outputFile.put(bitBuffer);
    }

    outputFile.close();
    std::cout << "Data written to file successfully with minimal bit size." << std::endl;

}

std::vector<int> Gif::convert_bit_to_int(const std::string &inputFilename, const std::vector<uint8_t> &bitCounts) {

    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << inputFilename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<int> decompressedData;
    uint8_t bitBuffer = 0;
    int bitPosition = 8;  // Đọc từng byte, nên khởi tạo 8 (tức là buffer trống)

    // Hàm đọc từng bit từ file
    auto readBit = [&]() -> int {
        if (bitPosition == 8) {  // Nếu đã đọc hết 1 byte, lấy byte tiếp theo
            inputFile.read(reinterpret_cast<char*>(&bitBuffer), 1);
            bitPosition = 0;
        }
        return (bitBuffer >> (7 - bitPosition++)) & 1;
    };

    // Duyệt qua danh sách `bitCounts` để đọc đúng số lượng bit
    for (uint8_t bitCount : bitCounts) {
        int number = 0;
        for (int i = 0; i < bitCount; ++i) {
            number = (number << 1) | readBit();  // Lấy từng bit và ghép thành số
        }
        decompressedData.push_back(number);
    }

    inputFile.close();
    return decompressedData;
}

void Gif::initialize_compress_Dictionary(std::unordered_map<std::string, int> &table) {
    table.clear();
    for (int i = 0; i <= 255; i++) {
        std::string ch = "";
        ch = char(i); // Tạo string từ ký tự ASCII
        table[ch] = i; // Gán string với giá trị ASCII tương ứng
    }

}

void Gif::initialize_decompress_Dictionary(std::unordered_map<int, std::string> &table) {
    table.clear();
    for (int i = 0; i <= 255; i++) {
        std::string ch = "";
        ch = char(i); //create string from ASCII character
        table[i] = ch; // assign string to dictionary with corresponding ASCII
    }
}

std::vector<unsigned char> Gif::get_gifData() const {
    return gifData;
}

std::vector<int> Gif::get_compressed_gifData() const {
    return compressed_gifData;
}

std::vector<unsigned char> Gif::get_decompressed_gifData() const {
    return decompressed_gifData;
}