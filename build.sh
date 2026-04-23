#!/bin/bash

# Build script for FileCompressor
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}========================================${NC}"
echo -e "${YELLOW}  FileCompressor Build Script${NC}"
echo -e "${YELLOW}========================================${NC}\n"

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake is not installed${NC}"
    echo "Install CMake from: https://cmake.org/download/"
    exit 1
fi

# Check if a C++ compiler is available
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo -e "${RED}Error: No C++ compiler found${NC}"
    echo "Install g++ or clang++ first"
    exit 1
fi

# Create build directory
echo -e "${YELLOW}[1/4]${NC} Creating build directory..."
mkdir -p build
cd build

# Configure CMake
echo -e "${YELLOW}[2/4]${NC} Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo -e "${YELLOW}[3/4]${NC} Building project..."
cmake --build . --config Release -j$(nproc)

# Verify build
echo -e "${YELLOW}[4/4]${NC} Verifying build..."
if [ -f "bin/compressor" ]; then
    echo -e "${GREEN}✓ Build successful!${NC}"
    echo -e "\n${GREEN}Executable location:${NC} $(pwd)/bin/compressor"
    echo -e "\n${YELLOW}Try it out:${NC}"
    echo "  ./bin/compressor --help"
else
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi
