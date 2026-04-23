#include "compressor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <iomanip>

// Run-Length Encoding compression
std::string Compressor::compressRLE(const std::string& input) {
    if (input.empty()) {
        return "";
    }
    
    std::stringstream compressed;
    size_t i = 0;
    
    while (i < input.length()) {
        char currentChar = input[i];
        size_t count = 1;
        
        // Count consecutive identical characters (max 255 to fit in one byte)
        while (i + count < input.length() && 
               input[i + count] == currentChar && 
               count < 255) {
            count++;
        }
        
        // Write: count (1 byte) + character (1 byte)
        compressed << static_cast<char>(count);
        compressed << currentChar;
        
        i += count;
    }
    
    return compressed.str();
}

// Run-Length Encoding decompression
std::string Compressor::decompressRLE(const std::string& input) {
    std::stringstream decompressed;
    
    for (size_t i = 0; i < input.length(); i += 2) {
        if (i + 1 < input.length()) {
            unsigned char count = static_cast<unsigned char>(input[i]);
            char ch = input[i + 1];
            
            for (unsigned char j = 0; j < count; ++j) {
                decompressed << ch;
            }
        }
    }
    
    return decompressed.str();
}

// Split file into chunks
std::vector<std::string> Compressor::splitFile(const std::string& filename, 
                                                size_t chunkSize) {
    std::vector<std::string> chunks;
    std::ifstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file: " << filename << std::endl;
        return chunks;
    }
    
    std::string buffer(chunkSize, '\0');
    
    while (file.read(buffer.data(), chunkSize) || file.gcount() > 0) {
        // Create a string with actual data read
        std::string chunk(buffer.data(), file.gcount());
        chunks.push_back(chunk);
    }
    
    file.close();
    return chunks;
}

// Merge compressed chunks into output file
void Compressor::mergeCompressedChunks(const std::string& filename,
                                       const std::vector<std::string>& compressedChunks) {
    std::ofstream outFile(filename, std::ios::binary);
    
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot create output file: " << filename << std::endl;
        return;
    }
    
    // Write header: number of chunks
    uint32_t numChunks = compressedChunks.size();
    outFile.write(reinterpret_cast<const char*>(&numChunks), sizeof(uint32_t));
    
    // Write each chunk with its size
    for (const auto& chunk : compressedChunks) {
        uint32_t chunkSize = chunk.length();
        outFile.write(reinterpret_cast<const char*>(&chunkSize), sizeof(uint32_t));
        outFile.write(chunk.c_str(), chunkSize);
    }
    
    outFile.close();
}

// Calculate compression ratio
double Compressor::calculateCompressionRatio(size_t originalSize, 
                                            size_t compressedSize) {
    if (originalSize == 0) {
        return 0.0;
    }
    
    return (1.0 - static_cast<double>(compressedSize) / originalSize) * 100.0;
}
