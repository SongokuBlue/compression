#include <bits/stdc++.h>
#include "stdio.h"
#include "set"
#include "GIF.h"
#include "Text.h"
#include "Video.h"
using namespace std;



std::string insertDecompressed(const std::string& filename) {
    std::string extensions[] = {".txt", ".mp4", ".gif"};

    // if "_compressed" exists, remove it
    std::string cleanFilename = filename;
    size_t compressedPos = cleanFilename.rfind("_compressed");
    if (compressedPos != std::string::npos) {
        cleanFilename.erase(compressedPos, 11); // Xóa "_compressed"
    }

    for (const auto& ext : extensions) {
        size_t pos = cleanFilename.rfind(ext);
        if (pos != std::string::npos) {
            return cleanFilename.substr(0, pos) + "_decompressed" + ext;
        }
    }
    return cleanFilename; // If there is no valid extension, returns the sanitized filename
}

void compressFolder(const std::string& inputFolder, const std::string& outputFolder) {
    std::filesystem::create_directories(outputFolder);
    for (const auto &entry : std::filesystem::directory_iterator(inputFolder)) {
        if(entry.is_regular_file() && entry.path().extension() == ".txt") { // checking if file inside whether it is text or not
            std::string inputFile = entry.path().string();
            std::string outputFile = outputFolder + "/" + entry.path().filename().string() + "_compressed";
            Text text;
            text.readTEXT(inputFile);
            string textData = text.get_textData();
            text.readTextAndCompress(textData,outputFile);
            std::cout << "Compressed: " << inputFile << " -> " << outputFile << std::endl;
        }else { // if not file text, not compress, just copy into a compress folder
            std::string output_non_textFile = outputFolder + "/" + entry.path().filename().string();
            std::filesystem::copy(entry, output_non_textFile, std::filesystem::copy_options::overwrite_existing);

        }
    }
}

void decompressFolder(const std::string& inputFolder, const std::string& outputFolder) {
    std::filesystem::create_directories(outputFolder);
    for (const auto &entry : std::filesystem::directory_iterator(inputFolder)) {
        std::string fileName = entry.path().filename().string();
        if(entry.is_regular_file() && fileName.find("_compressed") != std::string::npos) {
            std::string inputFile = entry.path().string();
            std::string outputFile = outputFolder + "/" + insertDecompressed(entry.path().filename().string());
            Text text;
            text.readTextAndDecompress(inputFile,outputFile);
            std::cout << "Decompressed: " << inputFile << " -> " << outputFile << std::endl;
        }else {
            std::string output_non_textFile = outputFolder + "/" + entry.path().filename().string();;
            std::filesystem::copy(entry, output_non_textFile, std::filesystem::copy_options::overwrite_existing);
        }
    }

}


int main(int argc, char* argv[])
{

    if(argc < 2) {
        std::cout << "Not enough file for compression";
        return 1;
    }

    for(int i = 1; i < argc; ++i) {
        std::string file_name = argv[i];
        std::filesystem::path inputPath(file_name);

        if(std::filesystem::is_directory(inputPath)) {
            if(file_name.find("_compressed") != std::string::npos) {
                size_t compressedPos = file_name.rfind("_compressed");
                std::filesystem::path decompressed_folder = inputPath.parent_path().parent_path() / ((inputPath.parent_path().filename().string().erase(compressedPos, 11) + "_decompressed"));
                decompressFolder(file_name, decompressed_folder);
                std::cout << "Folder " << file_name << "decompressed to: " << file_name + "_decompressed" << std::endl;
            }else {
                std::filesystem::path compressed_folder = inputPath.parent_path().parent_path()  /  (inputPath.parent_path().filename().string() + "_compressed");
                compressFolder(file_name, compressed_folder);
                std::cout << "Folder " << file_name << "compressed to: " << file_name + "_compressed" << std::endl;
            }

            continue;
        }
        if (file_name.ends_with(".txt")) {
            Text text;
            string Compressed_text = file_name + "_compressed";
            string Decompressed_text = insertDecompressed(file_name);
            text.readTEXT(file_name);
            string textData = text.get_textData();
            std::cout << "Encoding\n";
            text.readTextAndCompress(textData, Compressed_text);
            std::cout << "Decoding\n";
            text.readTextAndDecompress(Compressed_text,Decompressed_text);
            std::cout << "Decompress successfully\n";

        }else if (file_name.ends_with(".mp4")) {
            Video video;
            string Compressed_video = file_name + "_compressed";
            string Decompressed_video = insertDecompressed(file_name);
            video.readVideo(file_name);
            vector<unsigned char> videoData = video.get_videoData();
            vector<uint8_t> bitCounts;
            video.readVideoAndCompress(videoData,Compressed_video, bitCounts);
            vector<int> compressed_videoData = video.get_compressed_videoData();
            // converting uneven bits to integer require "bitCount" vector, which only achieved by reading original file
            // and using calculate bits function
            vector<int> converting_vector = video.convert_bit_to_int(Compressed_video, bitCounts);
            if (converting_vector == compressed_videoData) cout<< "Converting bit data to int data successfully";
            video.readVideoAndDecompress(converting_vector, Decompressed_video);
            vector<unsigned char> decompressed_videoData = video.get_decompressed_videoData();
            if(decompressed_videoData == videoData) { // check if these vector have the same data and element
                cout<<"decompress successfully" << "\n";
            }
        }else if (file_name.ends_with(".gif")) {
            Gif gif;
            string Compressed_gif = file_name + "_compressed";
            string Decompressed_gif = insertDecompressed(file_name);
            gif.readGIF(file_name);
            vector<unsigned char> gifData = gif.get_gifData();
            vector<uint8_t> bitCounts;
            gif.readGIFAndCompress(gifData,Compressed_gif, bitCounts);
            vector<int> compressed_gifData = gif.get_compressed_gifData();
            vector<int> converting_vector = gif.convert_bit_to_int(Compressed_gif, bitCounts);
            if (converting_vector == compressed_gifData) cout<< "Converting bit data to int data successfully";
            gif.readGIFAndDecompress(converting_vector, Decompressed_gif);
            vector<unsigned char> decompressed_gifData = gif.get_decompressed_gifData();
            if(decompressed_gifData == gifData) { // check if these vector have the same data and element
                cout<<"decompress successfully" << "\n";
            }
        }else {
            std::cerr << "Undefined file \n";
            return 1;
        }
    }


    return 0;

}
