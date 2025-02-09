#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <queue>
#include "fstream"
#include <bitset>
struct MinHeapNode {
    char data;
    int freq;
    MinHeapNode *left, *right;

    MinHeapNode(char data, int freq) {
        left = right = nullptr;
        this->data = data;
        this->freq = freq;
    }
};

struct compare {
    bool operator()(MinHeapNode* l, MinHeapNode* r) {
        return l->freq > r->freq;
    }

};

// Function declarations
MinHeapNode* HuffmanCodes(char data[], int freq[], int size, std::string input, const std::string& outputFileName);
std::string encode(std::string input, const std::unordered_map<char, std::string>& huffmanCode);
void printCodes(MinHeapNode* root, std::string str);
void storeCodes(struct MinHeapNode* root, std::unordered_map<char, std::string>& huffmanCode, std::string str);
void compressFile(const std::string& inputFileName, const std::string& outputFileName,MinHeapNode* &root);
void decompressFile(const std::string &inputFileName, const std::string &outputFileName, MinHeapNode* root);
std::string decode_file(struct MinHeapNode* root, std:: string encodedStr);
#endif // HUFFMAN_H