#ifndef HUFFMANIMAGE_H
#define HUFFMANIMAGE_H

#define BMP_HEADER_SIZE 54
#define MAX_GRAY_VALUE 256

#define COLOR_TABLE_SIZE 1024
#include "string"
#include "unordered_map"
struct pixfreq {
    int pix;
    float freq;
    struct pixfreq *left, *right;std:: string code ;
};

struct huffcode {

    float freq;
};

struct ImageData {
    unsigned char **image;
    int width, height;
    float histogram[MAX_GRAY_VALUE];
};

struct HuffmanTree {
    struct pixfreq *nodes[MAX_GRAY_VALUE * 2];
    int totalNodes;
};
struct ComparePixFreq {
    bool operator()(struct pixfreq* a, struct pixfreq* b) {
        return a->freq > b->freq;
    }
};

void readBMP(const char *filename, struct ImageData *data);
void computeHistogram(struct ImageData *data, float hist[MAX_GRAY_VALUE]);
void buildHuffmanTree(struct ImageData *data, struct HuffmanTree *tree,std::unordered_map<int, std::string> &huffmanMap);
void generateCodes(struct pixfreq *node, std::string code, std::unordered_map<int, std::string> &huffmanMap);
void encodeImage(const char *outputFile, struct ImageData *data, std::unordered_map<int, std::string> &huffmanMap);
void decodeImage(const char *inputFile, const char *outputFile, struct ImageData *data, const std::unordered_map<std::string,int>& huffmanMap);
#endif
