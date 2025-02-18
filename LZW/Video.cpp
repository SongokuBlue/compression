//
// Created by Admin on 1/29/2025.
//
#include "Video.h"

std::vector<int> Video::encoding(const std::vector<unsigned char> &s1) {
    std::cout << "Encoding\n";
    std::unordered_map<std::string, int> table;
    initialize_compress_Dictionary(table);

    std::string p = "", c = ""; // p: current string, c: is a next character
    p += s1[0];  // initialize the first string by plus with first character
    int code = 256; // code number (after maximum ASCII number)
    std::vector<int> output_code; // vector which stored output
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
            output_code.push_back(table[p]);
            table[p + c] = code; // adding p+c pattern with corresponding code number to dictionary
            code++; // increase code number by 1
            if(code >= 4096) { // if the code number out of 12 bit
                initialize_compress_Dictionary(table); // reset compress dictionary
                code = 256;
            }
            p = c; // setting current pattern be c
        }
        c = ""; // reset c before checking the next character
    }
    output_code.push_back(table[p]); // push the last pattern to output
    return output_code;
}

std::vector<unsigned char> Video::decoding(std::vector<int> op) {
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
        if (count >= 4096) { // if code number out of 12 bit, reset dictionary
            initialize_decompress_Dictionary(table);
            count = 256;
        }
        old = n; // update old to become previous code number
    }
    return decompressedData;

}

std::vector<unsigned char> Video::readVideo(const std::string &inputFilename) {
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Opening file " << inputFilename << " failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // reading the data from video file
    std::vector<unsigned char> gif_Data((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    videoData = gif_Data;
    inputFile.close();

    return videoData; // return read data
}


std::vector<int> Video::readVideoAndCompress(const std::vector<unsigned char> &videoData, const std::string &outputFilename, std::vector<uint8_t>& bitCounts) {
    compressed_videoData = encoding(videoData);
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if(!outputFile.is_open()) { // checking if output file is opened
        std::cerr << "Opening file " << outputFilename << "failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    writeBitsToFile(compressed_videoData, outputFilename, bitCounts);
    return compressed_videoData;
}

std::vector<unsigned char> Video::readVideoAndDecompress(const std::vector<int> compressed_videoData, const std::string &outputFilename) {
    decompressed_videoData = decoding(compressed_videoData);
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Opening file " << outputFilename << " failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    outputFile.write(reinterpret_cast<const char*>(decompressed_videoData.data()), decompressed_videoData.size());
    outputFile.close();

    std::cout << "Decompressed GIF written to " << outputFilename << " successfully." << std::endl;
    return decompressed_videoData;

}

int Video::calculate_bits(int code) {
    // Calculate the requirement bit with minimal bit size for each number
    return (code == 0) ? 1 : (int)log2(code + 1) + 1;
}

void Video::writeBitsToFile(const std::vector<int>& compressed_videoData, const std::string &outputFilename, std::vector<uint8_t>& bitCounts) {
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Opening file " << outputFilename << " failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    uint8_t bitBuffer = 0;  // 8-bit buffer
    int bitPosition = 0;     // Current bit position in the buffer

    for (int code : compressed_videoData) {
        int bitCount = calculate_bits(code);  // Number of bits required for the code
        bitCounts.push_back(bitCount);

        // Write the number of bits of this code to the file first (so that we know how many bits to read later)
        // outputFile.put(static_cast<uint8_t>(bitCount));

        for (int i = 0; i < bitCount; ++i) {
            int bit = (code >> (bitCount - 1 - i)) & 1;  // Extract bits from left to right
            bitBuffer |= (bit << (7 - bitPosition));  // Insert bit into the buffer
            bitPosition++;

            if (bitPosition == 8) {  // When the buffer is full (8 bits), write to file
                outputFile.put(bitBuffer);
                bitBuffer = 0;  // Reset buffer
                bitPosition = 0; // Reset bit position
            }
        }
    }

    // Write the remaining bits if there are leftover bits in the buffer
    if (bitPosition > 0) {
        outputFile.put(bitBuffer);
    }

    outputFile.close();
    std::cout << "Data written to file successfully with minimal bit size." << std::endl;
}


std::vector<int> Video::convert_bit_to_int(const std::string &inputFilename, const std::vector<uint8_t> &bitCounts) {

    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << inputFilename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<int> decompressedData;
    uint8_t bitBuffer = 0;
    int bitPosition = 8;  // Reading each byte, so initialize at 8 (meaning the buffer is empty)

    // Function to read each bit from the file
    auto readBit = [&]() -> int {
        if (bitPosition == 8) {  // If a byte is fully read, fetch the next byte
            inputFile.read(reinterpret_cast<char*>(&bitBuffer), 1);
            bitPosition = 0;
        }
        return (bitBuffer >> (7 - bitPosition++)) & 1;
    };

    // Iterate through `bitCounts` to read the correct number of bits
    for (uint8_t bitCount : bitCounts) {
        int number = 0;
        for (int i = 0; i < bitCount; ++i) {
            number = (number << 1) | readBit();  // Extract each bit and reconstruct the number
        }
        decompressedData.push_back(number);
    }

    inputFile.close();
    return decompressedData;
}



void Video::initialize_compress_Dictionary(std::unordered_map<std::string, int> &table) {
    table.clear();
    for (int i = 0; i <= 255; i++) {
        std::string ch = "";
        ch = char(i); // create string from ASCII character
        table[ch] = i; // Assign string with corresponding AsII character
    }

}

void Video::initialize_decompress_Dictionary(std::unordered_map<int, std::string> &table) {
    table.clear();
    for (int i = 0; i <= 255; i++) {
        std::string ch = "";
        ch = char(i); //create string from ASCII character
        table[i] = ch; // assign string to dictionary with corresponding ASCII
    }
}

std::vector<unsigned char> Video::get_videoData() const {
    return videoData;
}

std::vector<int> Video::get_compressed_videoData() const {
    return compressed_videoData;
}

std::vector<unsigned char> Video::get_decompressed_videoData() const {
    return decompressed_videoData;
}

