@echo off
REM Build script for FileCompressor on Windows

echo ========================================
echo   FileCompressor Build Script (Windows)
echo ========================================
echo.

REM Check if CMake is installed
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: CMake is not installed
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
)

REM Create build directory
echo [1/4] Creating build directory...
if not exist build mkdir build
cd build

REM Configure CMake with Visual Studio 2019 (adjust if needed)
echo [2/4] Configuring CMake...
cmake .. -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release

REM Build
echo [3/4] Building project...
cmake --build . --config Release

REM Verify build
echo [4/4] Verifying build...
if exist "bin\Release\compressor.exe" (
    echo.
    echo Build successful!
    echo.
    echo Executable: %cd%\bin\Release\compressor.exe
    echo.
    echo Try it out:
    echo   .\bin\Release\compressor.exe --help
) else (
    echo Build failed
    pause
    exit /b 1
)

pause
