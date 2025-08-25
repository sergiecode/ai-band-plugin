@echo off
echo AI Band Plugin - Test Runner
echo ============================

:: Check if we're in the right directory
if not exist "CMakeLists.txt" (
    echo Error: Please run this script from the ai-band-plugin root directory
    pause
    exit /b 1
)

echo.
echo Building test application...

:: Create build directory for tests
if not exist "build-tests" mkdir build-tests
cd build-tests

:: Configure with tests enabled
echo Configuring CMake with tests enabled...
cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug

if %errorlevel% neq 0 (
    echo.
    echo ERROR: CMake configuration failed.
    echo Make sure JUCE is properly installed and accessible.
    echo.
    pause
    exit /b 1
)

:: Build the test application
echo.
echo Building test application...
cmake --build . --config Debug --target AIBandPluginTests

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Build failed.
    echo Check the error messages above for details.
    echo.
    pause
    exit /b 1
)

:: Find the test executable
set TEST_EXE=""
if exist "Debug\AIBandPluginTests.exe" set TEST_EXE="Debug\AIBandPluginTests.exe"
if exist "AIBandPluginTests.exe" set TEST_EXE="AIBandPluginTests.exe"
if exist "AIBandPluginTests_artefacts\Debug\AIBandPluginTests.exe" set TEST_EXE="AIBandPluginTests_artefacts\Debug\AIBandPluginTests.exe"

if %TEST_EXE%=="" (
    echo.
    echo ERROR: Could not find test executable
    echo Looking in common build locations...
    dir /s AIBandPluginTests.exe
    pause
    exit /b 1
)

echo.
echo Running tests...
echo ================

:: Run the tests
%TEST_EXE% --report

set TEST_RESULT=%errorlevel%

echo.
echo ================
if %TEST_RESULT%==0 (
    echo ✓ ALL TESTS PASSED!
    echo.
    echo The AI Band Plugin is working correctly.
) else (
    echo ✗ SOME TESTS FAILED!
    echo.
    echo Please check the test output above for details.
    echo You may need to fix issues before the plugin will work properly.
)

:: Check for test report
if exist "test_report.txt" (
    echo.
    echo Test report generated: test_report.txt
    echo Opening report...
    notepad test_report.txt
)

echo.
echo Test run complete. Press any key to exit...
pause >nul

cd ..
exit /b %TEST_RESULT%
