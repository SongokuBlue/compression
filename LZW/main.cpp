#include <bits/stdc++.h>
#include "stdio.h"
#include "set"
using namespace std;
void initialize_compress_Dictionary(unordered_map<string, int>& table) {
    table.clear();
    for (int i = 0; i <= 255; i++) {
        string ch = "";
        ch = char(i); // Tạo string từ ký tự ASCII
        table[ch] = i; // Gán string với giá trị ASCII tương ứng
    }
}

void initialize_decompress_Dictionary(unordered_map<int, string>& table) {
    table.clear();
    for (int i = 0; i <= 255; i++) {
        string ch = "";
        ch = char(i); //create string from ASCII character
        table[i] = ch; // assign string to dictionary with corresponding ASCII
    }
}
int calculate_bits(int code) {
    // Tính số bit cần thiết để biểu diễn một mã
    return (code == 0) ? 1 : (int)log2(code + 1) + 1;
}

void writeBitsToFile(const vector<int>& compressed_gifData, const string& filename) {
    ofstream outputFile(filename, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Opening file " << filename << " failed!" << endl;
        exit(EXIT_FAILURE);
    }

    bitset<32> bitBuffer; // Dùng bitset để lưu trữ bit của các số.
    int bitPosition = 0;

    for (int code : compressed_gifData) {
        int bitCount = calculate_bits(code);  // Tính số bit của từng mã
        for (int i = bitCount - 1; i >= 0; --i) {
            bitBuffer[bitPosition] = (code >> i) & 1; // Gán từng bit vào buffer
            bitPosition++;
            if (bitPosition == 8) {  // Nếu buffer đầy 8 bit, ghi vào file
                outputFile.put(bitBuffer.to_ulong());  // Ghi byte hiện tại vào file
                bitBuffer.reset();  // Reset lại buffer
                bitPosition = 0;
            }
        }
    }

    // Ghi lại phần còn lại nếu có (nếu bitPosition > 0 thì có một byte chưa đầy)
    if (bitPosition > 0) {
        outputFile.put(bitBuffer.to_ulong());
    }

    outputFile.close();
    cout << "Data written to file successfully with minimal bit size." << endl;
}
vector<int> encoding(const vector<unsigned char>& s1)
{
    cout << "Encoding\n";
    unordered_map<string, int> table;
    initialize_compress_Dictionary(table);

    string p = "", c = ""; // p: current string, c: is a next character
    p += s1[0];  // initialize the first string by plus with first character
    int code = 256; // code number (after maximum ASCII number)
    vector<int> output_code; // vector which stored output
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

vector<unsigned char> decoding(vector<int> op)
{
    cout << "\nDecoding\n";
    unordered_map<int, string> table;
    //initialize dictionary with ASCII (0-255)
    initialize_decompress_Dictionary(table);

    vector<unsigned char> decompressedData;
    int old = op[0]; // get the first code number in encode output
    string s = table[old]; // searching dictionary to get first string (always start with character)
    string c ;
    c = s[0]; // c is a first character of a current pattern
//    cout << s;
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
        if (count >= 4096) {
            initialize_decompress_Dictionary(table);
            count = 256;
        }
        old = n; // update old to become previous code number
    }
    return decompressedData;
}


vector<unsigned char> readGIF(const string& inputFilename) {
    ifstream inputFile(inputFilename, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Opening file " << inputFilename << " failed" << endl;
        exit(EXIT_FAILURE);
    }

    // Đọc dữ liệu từ file GIF
    vector<unsigned char> gifData((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    return gifData; // Trả về dữ liệu đã đọc
}

void Write_uncompress_data(const vector<unsigned char>& gifData, const string& outputFilename) {
    ofstream outputFile(outputFilename, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Opening " << outputFilename << "failed" << endl;
        exit(EXIT_FAILURE);
    }


    for (unsigned char ch: gifData ) {
        outputFile.write(reinterpret_cast<char*>(&ch), sizeof(ch));
    }

    outputFile.close();
    cout << "Writing uncompress data to " << outputFilename << " Successfully";
}
vector<int> readGIFAndCompress (const vector<unsigned char>& gifData, const string& outputFilename) {
    vector<int> compressed_gifData = encoding(gifData);
    ofstream outputFile(outputFilename, ios::binary);
    if(!outputFile.is_open()) {
        cerr << "Opening file " << outputFilename << "failed" << endl;
        exit(EXIT_FAILURE);
    }

    for (int code : compressed_gifData) {
        outputFile << code; // Ghi từng mã dưới dạng nhị phân
    }
    outputFile.close();
    return compressed_gifData;
}

vector<unsigned char> readGIFAndDecompress(const vector<int> compressed_gifData, const string& outputFilename) {
    vector<unsigned char> decompressed_gifData = decoding(compressed_gifData);
    ofstream outputFile(outputFilename, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Opening file " << outputFilename << " failed" << endl;
        exit(EXIT_FAILURE);
    }

    outputFile.write(reinterpret_cast<const char*>(decompressed_gifData.data()), decompressed_gifData.size());
    outputFile.close();

    cout << "Decompressed GIF written to " << outputFilename << " successfully." << endl;
    return decompressed_gifData;
}

int main()
{

//    string s = "WYS*WYGWYS*WYSWYSG";
//    vector<int> output_code = encoding(s);
//    cout << "Output Codes are: ";
//    for (int i = 0; i < output_code.size(); i++) {
//        cout << output_code[i] << " ";
//    }
//    cout << endl;
//    decoding(output_code);
//    string outputFilename = "Output.txt";     // Tên file để ghi dữ liệu
//
//    readGIFAndWrite(inputFilename, outputFilename);
    string inputFilename = "Simpson.gif";
    vector<unsigned char> gifData = readGIF(inputFilename);
    string output = "Simpson_data_info.txt";
    Write_uncompress_data(gifData, output);
    string compress_file = "Compress_step1.txt";
    vector<int> compressed_gifData = readGIFAndCompress(gifData, compress_file);
    string decompress_file = "Decompress.txt";
    vector<unsigned char> decompressed_gifData = readGIFAndDecompress(compressed_gifData, decompress_file);
    string final_file = "Compress_step2.txt";
    writeBitsToFile(compressed_gifData,final_file);

    cout << "Original size (number of codes): " << gifData.size() << "\n";
    cout << "Original size:" << gifData.size() * sizeof(unsigned char) << "\n";
    cout << "Compressed size (number of codes): " << compressed_gifData.size() << "\n";
    cout << "Compressed file size: " << compressed_gifData.size() * sizeof(int) << " bytes\n";
    cout << "Decompress file size (number of codes): " << decompressed_gifData.size() << "\n";
    cout << "Decompress file size " << decompressed_gifData.size() * sizeof(unsigned char) << "\n";
    return 0;

}
