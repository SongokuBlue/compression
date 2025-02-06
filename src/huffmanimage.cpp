#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <bitset>
#include "huffman_image.h"

using namespace std;

MinHeapNodeImage::MinHeapNodeImage(unsigned int data, unsigned freq) {
    this->data = data;
    this->freq = freq;
    this->left = nullptr;
    this->right = nullptr;
}

bool compareImage::operator()(MinHeapNodeImage* l, MinHeapNodeImage* r) {
    return l->freq > r->freq;  // Compare nodes based on frequency
}
void storeCodes(MinHeapNodeImage* root, unordered_map<unsigned int, string>& huffmanCode, string str) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanCode[root->data] = str;
    }
    storeCodes(root->left, huffmanCode, str + "0");
    storeCodes(root->right, huffmanCode, str + "1");
}

MinHeapNodeImage* buildHuffmanTree(vector<unsigned int>& data, vector<unsigned>& freq) {
    priority_queue<MinHeapNodeImage*, vector<MinHeapNodeImage*>, compareImage> minHeap;
    for (size_t i = 0; i < data.size(); i++) {
        minHeap.push(new MinHeapNodeImage(data[i], freq[i]));
    }
    while (minHeap.size() > 1) {
        MinHeapNodeImage *left = minHeap.top(); minHeap.pop();
        MinHeapNodeImage *right = minHeap.top(); minHeap.pop();
        MinHeapNodeImage *top = new MinHeapNodeImage('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
    return minHeap.top();
}

void compressImage(const string& inputFileName, const string& outputFileName, MinHeapNodeImage*& root) {
    ifstream inputFile(inputFileName, ios::binary);
    if (!inputFile) {
        cerr << "Cannot open file " << inputFileName << endl;
        return;
    }
    vector<unsigned char> imageData((istreambuf_iterator<char>(inputFile)), {});
    inputFile.close();

    unordered_map<unsigned char, unsigned> freqMap;
    for (unsigned char byte : imageData) {
        freqMap[byte]++;
    }

    vector<unsigned int> data;
    vector<unsigned> freq;
    for (const auto& pair : freqMap) {
        data.push_back(pair.first);
        freq.push_back(pair.second);
    }

    root = buildHuffmanTree(data, freq);
    unordered_map<unsigned int, string> huffmanCode;
    storeCodes(root, huffmanCode, "");

    string encodedStr = "";
    for (unsigned char byte : imageData) {
        encodedStr += huffmanCode[byte];
    }

    ofstream outputFile(outputFileName, ios::binary);
    if (!outputFile) {
        cerr << "Cannot write to file " << outputFileName << endl;
        return;
    }

    int remainder = encodedStr.length() % 8;
    if (remainder != 0) {
        encodedStr.append(8 - remainder, '0');
    }
    outputFile.put(static_cast<unsigned char>(remainder));
    for (size_t i = 0; i < encodedStr.length(); i += 8) {
        bitset<8> byte(encodedStr.substr(i, 8));
        outputFile.put(static_cast<unsigned char>(byte.to_ulong()));
    }
    outputFile.close();
}

string decodeHuffman(MinHeapNodeImage* root, string encodedStr) {
    string decodedStr = "";
    MinHeapNodeImage* current = root;
    for (char bit : encodedStr) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decodedStr += current->data;
            current = root;
        }
    }
    return decodedStr;
}

void decompressImage(const string& inputFileName, const string& outputFileName, MinHeapNodeImage* root) {
    ifstream inputFile(inputFileName, ios::binary);
    if (!inputFile) {
        cerr << "Cannot open file " << inputFileName << endl;
        return;
    }
    unsigned char remainder;
    inputFile.read(reinterpret_cast<char*>(&remainder), sizeof(remainder));
    string encodedStr = "";
    unsigned char byte;
    while (inputFile.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        bitset<8> bits(byte);
        encodedStr += bits.to_string();
    }
    inputFile.close();
    if (remainder > 0) {
        encodedStr.erase(encodedStr.end() - (8 - remainder), encodedStr.end());
    }
    string decodedData = decodeHuffman(root, encodedStr);
    ofstream outputFile(outputFileName, ios::binary);
    if (!outputFile) {
        cerr << "Cannot write to file " << outputFileName << endl;
        return;
    }
    outputFile.write(decodedData.data(), decodedData.size());
    outputFile.close();
}