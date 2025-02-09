#include <iostream>
#include <unordered_map>
#include <queue>
#include <string>
#include "huffman.h"
using namespace std;
struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    return temp;
}



// Function to encode the input string using Huffman codes
std::string encode(std::string input, const std::unordered_map<char, std::string>& huffmanCode) {
    string encodedStr = "";
    for (char c : input) {
        encodedStr += huffmanCode.at(c);
    }
    return encodedStr;
}
void storeCodes(struct MinHeapNode* root, unordered_map<char, string>& huffmanCode, string str) {
    if (root == NULL)
        return;

    if (root->data != '$') {
        huffmanCode[root->data] = str;
    }


    storeCodes(root->left, huffmanCode, str + "0");
    storeCodes(root->right, huffmanCode, str + "1");
}
// Function to generate Huffman Codes and encode input
MinHeapNode* HuffmanCodes(char data[], int freq[], int size, std::string input, const std::string& outputFileName) {
    priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;

    // Step 1: Create a node for each character and add it to the priority queue
    for (int i = 0; i < size; ++i) {
        minHeap.push(new MinHeapNode(data[i], freq[i]));
    }

    // Step 2: Build the Huffman tree
    while (minHeap.size() > 1) {
        MinHeapNode* left = minHeap.top(); minHeap.pop();
        MinHeapNode* right = minHeap.top(); minHeap.pop();

        MinHeapNode* top = new MinHeapNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    MinHeapNode* root = minHeap.top();

    // Step 3: Generate Huffman codes and store them in the map
    unordered_map<char, string> huffmanCode;
    storeCodes(root, huffmanCode, "");

    // Step 4: Encode the input string
    string encodedStr = encode(input, huffmanCode);

    // Step 5: Save the encoded string to output file
    ofstream outputFile(outputFileName, ios::binary);
    if (outputFile.is_open()) {
        // Calculate remainder bits
        int remainder = encodedStr.length() % 8;
        if (remainder != 0) {
            encodedStr.append(8 - remainder, '0'); // Pad with '0' bits
        }

        // Write the remainder to the file
        outputFile.put(static_cast<unsigned char>(remainder)); // Write remainder

        // Write binary data to the file
        for (size_t i = 0; i < encodedStr.length(); i += 8) {
            bitset<8> byte(encodedStr.substr(i, 8));
            outputFile.put(static_cast<unsigned char>(byte.to_ulong()));
        }

        outputFile.close();
        cout << "Encoded string saved to file: " << outputFileName << endl;
    } else {
        cerr << "Error: Cannot open output file " << outputFileName << endl;
    }

    // Return the root of the Huffman tree to use in decompression
    return root;
}

void compressFile(const string &inputFileName, const string &outputFileName,MinHeapNode*& root) {
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Cannot open file " << inputFileName << std::endl;
        return;
    }
    std::string inputData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    // calculate character frequency
    std::unordered_map<char, int> freqMap;
    for (char ch : inputData) {
        freqMap[ch]++;
    }
    std::vector<char> data;
    std::vector<int> freq;
    for (const auto& pair : freqMap) {
        data.push_back(pair.first);
        freq.push_back(pair.second);
    }

    root= HuffmanCodes(data.data(), freq.data(), data.size(), inputData, outputFileName);
}
string decode_file(struct MinHeapNode* root, string encodedStr)
{
    string ans = "";
    struct MinHeapNode* current = root;
    for (char bit : encodedStr) {

        if (bit == '0')
            current = current->left;
        else
            current = current->right;


        if (current->left == NULL && current->right == NULL) {
            ans += current->data;
            current = root;
        }
    }
    return ans;
}

void decompressFile(const string& inputFileName, const string& outputFileName, MinHeapNode* root) {
    ifstream inputFile(inputFileName, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Error: Cannot open file " << inputFileName << endl;
        return;
    }

    // Read the remainder bits from the file
    unsigned char remainder;
    inputFile.read(reinterpret_cast<char*>(&remainder), sizeof(remainder));

    string encodedStr = "";
    unsigned char byte;

    // Read binary data from the file and convert to bit string
    while (inputFile.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        bitset<8> bits(byte);
        encodedStr += bits.to_string();
    }
    inputFile.close();

    // Remove the padding bits at the end
    if (remainder > 0) {
        encodedStr.erase(encodedStr.end() - (8 - remainder), encodedStr.end());
    }

    // Decode the bit string using the Huffman tree
    string decodedStr = decode_file(root, encodedStr);

    // Write the decoded string to the output file
    ofstream outputFile(outputFileName, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Error: Cannot write to file " << outputFileName << endl;
        return;
    }

    outputFile << decodedStr;
    outputFile.close();

    cout << "File decompressed successfully: " << outputFileName << endl;
}