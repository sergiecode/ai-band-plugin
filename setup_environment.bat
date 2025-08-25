@echo off
echo AI Band Plugin - Development Setup Helper
echo ==========================================

echo.
echo This script will help you set up the development environment for the AI Band Plugin.
echo.

:: Check for Visual Studio
echo Checking for development tools...

:: Look for Visual Studio 2022
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" (
    echo ✅ Visual Studio 2022 Community found
    set VS_FOUND=1
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" (
    echo ✅ Visual Studio 2022 Professional found
    set VS_FOUND=1
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\devenv.exe" (
    echo ✅ Visual Studio 2022 Enterprise found
    set VS_FOUND=1
) else (
    echo ❌ Visual Studio 2022 not found
    set VS_FOUND=0
)

:: Look for Visual Studio 2019
if %VS_FOUND%==0 (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe" (
        echo ✅ Visual Studio 2019 Community found
        set VS_FOUND=1
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.exe" (
        echo ✅ Visual Studio 2019 Professional found
        set VS_FOUND=1
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv.exe" (
        echo ✅ Visual Studio 2019 Enterprise found
        set VS_FOUND=1
    )
)

:: Check for JUCE
echo.
echo Checking for JUCE Framework...

if exist "C:\JUCE\projucer.exe" (
    echo ✅ JUCE found in C:\JUCE\
    set JUCE_FOUND=1
) else if exist "C:\Program Files\JUCE\projucer.exe" (
    echo ✅ JUCE found in C:\Program Files\JUCE\
    set JUCE_FOUND=1
) else (
    echo ❌ JUCE not found
    set JUCE_FOUND=0
)

:: Check for CMake
echo.
echo Checking for CMake...
cmake --version >nul 2>&1
if %errorlevel%==0 (
    echo ✅ CMake found
    cmake --version
    set CMAKE_FOUND=1
) else (
    echo ❌ CMake not found
    set CMAKE_FOUND=0
)

:: Summary and recommendations
echo.
echo ==========================================
echo SETUP SUMMARY
echo ==========================================

if %VS_FOUND%==1 (
    echo ✅ Visual Studio: Available
) else (
    echo ❌ Visual Studio: NOT FOUND
    echo    Download from: https://visualstudio.microsoft.com/downloads/
    echo    Make sure to install "Desktop development with C++" workload
)

if %JUCE_FOUND%==1 (
    echo ✅ JUCE Framework: Available
) else (
    echo ❌ JUCE Framework: NOT FOUND
    echo    Download from: https://juce.com/get-juce
    echo    Install to C:\JUCE\ (recommended)
)

if %CMAKE_FOUND%==1 (
    echo ✅ CMake: Available
) else (
    echo ⚠️  CMake: NOT FOUND (Optional but recommended)
    echo    Download from: https://cmake.org/download/
)

echo.
echo ==========================================
echo NEXT STEPS
echo ==========================================

if %VS_FOUND%==0 (
    echo 1. Install Visual Studio 2019 or 2022 with C++ tools
    echo    - Go to https://visualstudio.microsoft.com/downloads/
    echo    - Choose Community edition (free)
    echo    - During installation, select "Desktop development with C++"
    echo.
)

if %JUCE_FOUND%==0 (
    echo 2. Install JUCE Framework
    echo    - Go to https://juce.com/get-juce
    echo    - Download and install to C:\JUCE\
    echo    - This includes the Projucer application
    echo.
)

if %CMAKE_FOUND%==0 (
    echo 3. Install CMake (Optional)
    echo    - Go to https://cmake.org/download/
    echo    - Add to PATH during installation
    echo.
)

echo 4. Configure VS Code for C++
echo    - Install C/C++ extension by Microsoft
echo    - Press Ctrl+Shift+P and run "C/C++: Select IntelliSense Configuration"
echo    - Choose your Visual Studio installation
echo.

if %JUCE_FOUND%==1 if %VS_FOUND%==1 (
    echo 🎉 READY TO BUILD!
    echo.
    echo To build the plugin:
    echo 1. Open AIBandPlugin.jucer in Projucer
    echo 2. Set JUCE modules path if needed
    echo 3. Click "Save Project" to generate Visual Studio files
    echo 4. Open the generated .sln file in Visual Studio
    echo 5. Build with Ctrl+Shift+B
) else (
    echo ⚠️  Install missing components first, then run this script again
)

echo.
echo ==========================================
echo TESTING THE PLUGIN LOGIC
echo ==========================================
echo.
echo While setting up the full environment, you can test the core logic:
echo.

if %VS_FOUND%==1 (
    echo Option 1: Compile simple test with Visual Studio
    echo    1. Open "Developer Command Prompt for VS"
    echo    2. Navigate to this directory
    echo    3. Run: cl /EHsc simple_test.cpp
    echo    4. Run: simple_test.exe
    echo.
)

echo Option 2: Use an online C++ compiler
echo    1. Copy the contents of simple_test.cpp
echo    2. Paste into https://onlinegdb.com/online_c++_compiler
echo    3. Click "Run" to test the core algorithms
echo.

echo Option 3: Install MinGW for g++
echo    1. Download from: https://www.mingw-w64.org/
echo    2. Add to PATH
echo    3. Run: g++ -o simple_test simple_test.cpp
echo    4. Run: simple_test.exe

echo.
pause
