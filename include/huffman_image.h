#ifndef HUFFMAN_IMAGE_H
#define HUFFMAN_IMAGE_H

#include <unordered_map>
#include <string>
#include <vector>
#include <queue>

using namespace std;

// C?u tr�c cho c�y Huffman
struct MinHeapNodeImage {
    unsigned int data;
    unsigned freq;
    MinHeapNodeImage* left;
    MinHeapNodeImage* right;

    MinHeapNodeImage(unsigned int data, unsigned freq);
};

// So s�nh hai node trong c�y Huffman
struct compareImage {
    bool operator()(MinHeapNodeImage* l, MinHeapNodeImage* r);
};

// Khai b�o c�c h�m
void storeCodes(MinHeapNodeImage* root, unordered_map<unsigned int, string>& huffmanCode, string str);
MinHeapNodeImage* buildHuffmanTree(vector<unsigned int>& data, vector<unsigned>& freq);
void writeBitsToFile(ofstream& outputFile, const string& encodedStr);
void compressImage(const string& inputFileName, const string& outputFileName, MinHeapNodeImage*& root);
std::string decodeHuffman(MinHeapNodeImage* root, string encodedStr);
void decompressImage(const string& inputFileName, const string& outputFileName, MinHeapNodeImage* root);

#endif
