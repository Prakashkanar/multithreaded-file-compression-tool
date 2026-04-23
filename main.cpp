#include "compressor.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <iomanip>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;



class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex taskMutex;
    std::condition_variable cv;
    bool shutdown = false;
    size_t activeTasks = 0;

public:
    explicit ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(taskMutex);

                        cv.wait(lock, [this] {
                            return shutdown || !tasks.empty();
                        });

                        if (shutdown && tasks.empty())
                            return;

                        task = std::move(tasks.front());
                        tasks.pop();
                        activeTasks++;
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(taskMutex);
                        activeTasks--;
                        cv.notify_all();
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(taskMutex);
            shutdown = true;
        }

        cv.notify_all();

        for (auto& worker : workers)
            if (worker.joinable()) worker.join();
    }

    template<typename F>
    void enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(taskMutex);
            tasks.emplace(std::forward<F>(task));
        }
        cv.notify_one();
    }

    void waitAll() {
        std::unique_lock<std::mutex> lock(taskMutex);
        cv.wait(lock, [this] {
            return tasks.empty() && activeTasks == 0;
        });
    }
};




void printUsage(const char* programName) {
    std::cout << "\n========== Multithreaded File Compression Tool ==========" << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS] <input_file> <output_file>" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -t, --threads <num>     Number of threads (default: CPU count)" << std::endl;
    std::cout << "  -c, --chunk-size <size> Chunk size in KB (default: 256 KB)" << std::endl;
    std::cout << "  -d, --decompress        Decompress instead of compress" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  " << programName << " -t 4 -c 512 input.txt output.rle" << std::endl;
    std::cout << "  " << programName << " -d output.rle decompressed.txt" << std::endl;
    std::cout << "========================================================\n" << std::endl;
}

void printStatistics(const std::string& inputFile, const std::string& outputFile,
                     double elapsedTime, size_t originalSize, size_t compressedSize,
                     int numThreads, size_t chunkSize) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "  COMPRESSION STATISTICS" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    std::cout << "\nFiles:" << std::endl;
    std::cout << "  Input:  " << inputFile << std::endl;
    std::cout << "  Output: " << outputFile << std::endl;
    
    std::cout << "\nData Sizes:" << std::endl;
    std::cout << "  Original:   " << std::setw(15) << originalSize << " bytes" << std::endl;
    std::cout << "  Compressed: " << std::setw(15) << compressedSize << " bytes" << std::endl;
    
    double ratio = Compressor::calculateCompressionRatio(originalSize, compressedSize);
    std::cout << "  Ratio:      " << std::setw(15) << std::fixed << std::setprecision(2) 
              << ratio << "%" << std::endl;
    
    std::cout << "\nPerformance:" << std::endl;
    std::cout << "  Threads:     " << std::setw(15) << numThreads << std::endl;
    std::cout << "  Chunk Size:  " << std::setw(15) << chunkSize / 1024 << " KB" << std::endl;
    std::cout << "  Time:        " << std::setw(15) << std::fixed << std::setprecision(3) 
              << elapsedTime << " seconds" << std::endl;
    
    double throughput = originalSize / (elapsedTime * 1024 * 1024); // MB/s
    std::cout << "  Throughput:  " << std::setw(15) << throughput << " MB/s" << std::endl;
    
    std::cout << std::string(50, '=') << "\n" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string inputFile, outputFile;
    int numThreads = std::thread::hardware_concurrency();
    size_t chunkSize = 256 * 1024; // 256 KB default
    bool decompress = false;
    
    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-t" || arg == "--threads") {
            if (i + 1 < argc) {
                numThreads = std::atoi(argv[++i]);
            }
        } else if (arg == "-c" || arg == "--chunk-size") {
            if (i + 1 < argc) {
                chunkSize = std::atoi(argv[++i]) * 1024; // Convert KB to bytes
            }
        } else if (arg == "-d" || arg == "--decompress") {
            decompress = true;
        } else if (arg[0] != '-') {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                outputFile = arg;
            }
        }
    }
    
    // Validate arguments
    if (inputFile.empty() || outputFile.empty()) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Check if input file exists
    if (!fs::exists(inputFile)) {
        std::cerr << "Error: Input file not found: " << inputFile << std::endl;
        return 1;
    }
    
    // Get input file size
    size_t originalSize = fs::file_size(inputFile);
    
    std::cout << "\n>>> Starting File Compression Tool <<<" << std::endl;
    std::cout << "Input file: " << inputFile << " (" << originalSize << " bytes)" << std::endl;
    std::cout << "Threads: " << numThreads << " | Chunk size: " << chunkSize / 1024 << " KB" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (decompress) {
        // Decompression logic (simplified)
        std::cout << "\nDecompression not fully implemented in this version." << std::endl;
        return 0;
    }
    
    // ===== COMPRESSION PHASE =====
    std::cout << "\n[1/3] Splitting file into chunks..." << std::endl;
    auto chunks = Compressor::splitFile(inputFile, chunkSize);
    std::cout << "      Chunks created: " << chunks.size() << std::endl;
    
    // Create thread pool
    std::cout << "\n[2/3] Compressing chunks with " << numThreads << " threads..." << std::endl;
    ThreadPool pool(numThreads);
    
    std::vector<std::string> compressedChunks(chunks.size());
    std::mutex resultMutex;
    size_t completedChunks = 0;
    
    // Enqueue compression tasks
    for (size_t i = 0; i < chunks.size(); ++i) {
        pool.enqueue([i, &chunks, &compressedChunks, &resultMutex, &completedChunks] {
            std::string compressed = Compressor::compressRLE(chunks[i]);
            {
                std::lock_guard<std::mutex> lock(resultMutex);
                compressedChunks[i] = compressed;
                completedChunks++;
                if (completedChunks % 10 == 0 || completedChunks == chunks.size()) {
                    std::cout << "      Progress: " << completedChunks << "/" 
                              << chunks.size() << " chunks" << std::endl;
                }
            }
        });
    }
    
    pool.waitAll();
    std::cout << "      All chunks compressed!" << std::endl;
    
    // Merge compressed chunks
    std::cout << "\n[3/3] Merging compressed chunks..." << std::endl;
    Compressor::mergeCompressedChunks(outputFile, compressedChunks);
    std::cout << "      Output file created: " << outputFile << std::endl;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate sizes
    size_t compressedSize = fs::file_size(outputFile);
    
    // Print statistics
    printStatistics(inputFile, outputFile, elapsedTime, originalSize, 
                   compressedSize, numThreads, chunkSize);
    
    return 0;
}
