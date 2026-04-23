#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <string>
#include <vector>
#include <memory>

class Compressor {
public:
    /**
     * Compresses data using Run-Length Encoding (RLE)
     * @param input The input data to compress
     * @return Compressed data as a string
     */
    static std::string compressRLE(const std::string& input);
    
    /**
     * Decompresses RLE compressed data
     * @param input The compressed data
     * @return Decompressed data
     */
    static std::string decompressRLE(const std::string& input);
    
    /**
     * Splits a large file into chunks
     * @param filename Path to the input file
     * @param chunkSize Size of each chunk in bytes
     * @return Vector of chunk data
     */
    static std::vector<std::string> splitFile(const std::string& filename, 
                                              size_t chunkSize);
    
    /**
     * Writes compressed chunks to output file
     * @param filename Path to output file
     * @param compressedChunks Vector of compressed chunk data
     */
    static void mergeCompressedChunks(const std::string& filename,
                                      const std::vector<std::string>& compressedChunks);
    
    /**
     * Calculates compression ratio
     * @param originalSize Original data size
     * @param compressedSize Compressed data size
     * @return Compression ratio as percentage
     */
    static double calculateCompressionRatio(size_t originalSize, 
                                           size_t compressedSize);
};

#endif // COMPRESSOR_H
