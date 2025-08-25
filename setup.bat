@echo off
echo AI Band Plugin - Quick Start Script
echo ===================================

echo.
echo Checking for required tools...

:: Check for CMake
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake not found. Please install CMake 3.22 or later.
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
)
echo ✓ CMake found

:: Check for Git
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Git not found. Please install Git.
    echo Download from: https://git-scm.com/download/win
    pause
    exit /b 1
)
echo ✓ Git found

:: Check for Visual Studio or Build Tools
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo WARNING: Visual Studio C++ compiler not found in PATH.
    echo Please ensure Visual Studio 2019/2022 with C++ development tools is installed.
    echo You may need to run this from a Visual Studio Developer Command Prompt.
    echo.
)

echo.
echo Setting up build environment...

:: Create build directory
if not exist "build" mkdir build

:: Navigate to build directory
cd build

echo.
echo Configuring project with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release

if %errorlevel% neq 0 (
    echo.
    echo ERROR: CMake configuration failed.
    echo This likely means JUCE is not installed or not found.
    echo.
    echo Please install JUCE:
    echo 1. Download JUCE from https://juce.com/get-juce
    echo 2. Install JUCE to a standard location
    echo 3. Ensure JUCE modules are accessible to CMake
    echo.
    echo Alternative: Use the Projucer method described in README.md
    pause
    exit /b 1
)

echo.
echo Configuration successful! 
echo.
echo Next steps:
echo 1. Build the project: cmake --build . --config Release
echo 2. Install JUCE if not already installed
echo 3. Load the plugin in your DAW
echo.
echo To build now, press any key...
pause >nul

echo.
echo Building project...
cmake --build . --config Release

if %errorlevel% eq 0 (
    echo.
    echo ✓ Build successful!
    echo.
    echo Plugin files should be in the build directory.
    echo Copy the .vst3 file to your DAW's plugin directory.
    echo.
    echo Windows VST3 location: C:\Program Files\Common Files\VST3\
    echo.
) else (
    echo.
    echo Build failed. Check the error messages above.
    echo.
    echo Common issues:
    echo - JUCE not installed or not found
    echo - Missing Visual Studio C++ build tools
    echo - Incorrect CMake configuration
    echo.
    echo See README.md for detailed setup instructions.
)

pause
