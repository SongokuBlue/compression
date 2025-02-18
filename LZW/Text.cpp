#include "Text.h"
std::vector<int> Text::encoding(const std::string& s1) {
    std::unordered_map<std::string, int> table;
    initialize_compress_Dictionary(table);

    std::string p = "", c = ""; // p: current string, c: is a next character
    p += s1[0];  // initialize the first string by plus with first character
    int code = 256; // code number (after maximum ASCII number)
    std::vector<int> output_code; // vector which stored output
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
            p = c; // setting current pattern be c
        }
        c = ""; // reset c before checking the next character
    }
    output_code.push_back(table[p]); // push the last pattern to output
    return output_code;
}

std::string Text::decoding(std::vector<int> op) {
    std::unordered_map<int, std::string> table;
    //initialize dictionary with ASCII (0-255)
    initialize_decompress_Dictionary(table);

    std::string decompressedData;
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
        decompressedData += s;
        c = ""; // reset c before set it to be a first character in a next pattern
        c += s[0]; // get the first character of current pattern
        table[count] = table[old] + c; // adding new string to dictionary
        count++; // increase code number by 1
        old = n; // update old to become previous code number
    }
    return decompressedData;

}

std::string Text::readTEXT(const std::string& inputFilename) {
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Opening file " << inputFilename << " failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Reading data from text file
    textData = std::string (std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
    return textData; // Return read data by using iterator
}



std::vector<int> Text::readTextAndCompress(const std::string& textData, const std::string& outputFilename) {
    compressed_textData = encoding(textData);
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if(!outputFile.is_open()) {
        std::cerr << "Opening file " << outputFilename << "failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    outputFile.write(reinterpret_cast<const char*>(compressed_textData.data()), compressed_textData.size() * sizeof(int));

    outputFile.close();
    return compressed_textData;
}

std::string Text::readTextAndDecompress (const std::string& inputFilename ,const std::string& outputFilename) {
    std::ifstream input(inputFilename, std::ios::binary);

    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open() || !input.is_open()) { // checking if both 2 files are opened
        std::cerr << "Opening file " << outputFilename << " failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    input.seekg(0, std::ios::end); // moving pointer to the end of file
    size_t file_size = input.tellg(); // for getting file size
    input.seekg(0); // return pointer back to the header for start reading a file
    std::vector<int> buffer(file_size / sizeof(int));
    input.read(reinterpret_cast<char*>(buffer.data()), file_size);
    decompressed_textData = decoding(buffer);
    outputFile << decompressed_textData;
    outputFile.close();
    return decompressed_textData;
}

void Text::initialize_compress_Dictionary(std::unordered_map<std::string, int>& table) {
    table.clear();
    for (int i = 0; i <= 255; i++) {
        std::string ch = "";
        ch = char(i);
        table[ch] = i;
    }
}
void Text::initialize_decompress_Dictionary(std::unordered_map<int, std::string>& table) {
    table.clear();
    for (int i = 0; i <= 255; i++) {
        std::string ch = "";
        ch = char(i); //create string from ASCII character
        table[i] = ch; // assign string to dictionary with corresponding ASCII
    }
}

std::string Text::get_textData() const {
    return textData;
}

std::vector<int> Text::get_compressed_textData() const {
    return compressed_textData;
}

std::string Text::get_decompressed_textData() const {
    return decompressed_textData;
}