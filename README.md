# Multithreaded File Compression Tool

This project is a command-line file compression tool built in C++.  
I created it to learn multithreading, synchronization, and performance optimization using C++.

The tool reads a text file, splits it into chunks, and compresses those chunks in parallel using multiple threads.

---

## Why I Built This

I wanted hands-on experience with:

- C++ system programming
- Multithreading using `std::thread`
- Mutexes and condition variables
- File handling
- CMake build system
- Debugging concurrency issues

---

## Features

- Compresses large text/log files
- Uses multiple worker threads
- Chunk-based parallel processing
- Progress tracking during compression
- Configurable thread count
- Built with CMake

---

## Tech Stack

- C++17
- STL Threads
- Mutex / Condition Variable
- File Streams
- CMake
- GCC / G++

---

## Project Structure

```text
main.cpp          -> CLI and workflow
compressor.cpp    -> Compression logic
compressor.h      -> Header file
threadpool.cpp    -> Thread pool implementation
CMakeLists.txt    -> Build config