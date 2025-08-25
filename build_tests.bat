@echo off
echo Building AI Band Plugin Tests...

:: Create build directory
if not exist "build" mkdir build
cd build

:: Configure CMake with tests enabled
cmake .. -DBUILD_TESTS=ON

:: Build the project
cmake --build . --config Debug

echo.
if %ERRORLEVEL% EQU 0 (
    echo ✅ Build completed successfully!
    echo Run tests with: run_tests.bat
) else (
    echo ❌ Build failed with error code %ERRORLEVEL%
)

cd ..
pause
