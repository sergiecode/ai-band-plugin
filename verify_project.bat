@echo off
echo AI Band Plugin - Code Validation
echo ==================================

echo.
echo Checking project structure...

:: Check main source files
if exist "Source\PluginProcessor.cpp" (
    echo ✅ PluginProcessor.cpp found
) else (
    echo ❌ PluginProcessor.cpp missing
)

if exist "Source\PluginProcessor.h" (
    echo ✅ PluginProcessor.h found
) else (
    echo ❌ PluginProcessor.h missing
)

if exist "Source\PluginEditor.cpp" (
    echo ✅ PluginEditor.cpp found
) else (
    echo ❌ PluginEditor.cpp missing
)

if exist "Source\PluginEditor.h" (
    echo ✅ PluginEditor.h found
) else (
    echo ❌ PluginEditor.h missing
)

if exist "Source\MidiManager.cpp" (
    echo ✅ MidiManager.cpp found
) else (
    echo ❌ MidiManager.cpp missing
)

if exist "Source\MidiManager.h" (
    echo ✅ MidiManager.h found
) else (
    echo ❌ MidiManager.h missing
)

if exist "Source\NetworkClient.cpp" (
    echo ✅ NetworkClient.cpp found
) else (
    echo ❌ NetworkClient.cpp missing
)

if exist "Source\NetworkClient.h" (
    echo ✅ NetworkClient.h found
) else (
    echo ❌ NetworkClient.h missing
)

echo.
echo Checking test files...

if exist "Tests\TestFramework.cpp" (
    echo ✅ TestFramework.cpp found
) else (
    echo ❌ TestFramework.cpp missing
)

if exist "Tests\TestFramework.h" (
    echo ✅ TestFramework.h found
) else (
    echo ❌ TestFramework.h missing
)

if exist "Tests\MidiManagerTests.cpp" (
    echo ✅ MidiManagerTests.cpp found
) else (
    echo ❌ MidiManagerTests.cpp missing
)

if exist "Tests\PluginProcessorTests.cpp" (
    echo ✅ PluginProcessorTests.cpp found
) else (
    echo ❌ PluginProcessorTests.cpp missing
)

if exist "Tests\TestRunner.cpp" (
    echo ✅ TestRunner.cpp found
) else (
    echo ❌ TestRunner.cpp missing
)

if exist "Tests\TestApplication.cpp" (
    echo ✅ TestApplication.cpp found
) else (
    echo ❌ TestApplication.cpp missing
)

echo.
echo Checking build configuration...

if exist "CMakeLists.txt" (
    echo ✅ CMakeLists.txt found
) else (
    echo ❌ CMakeLists.txt missing
)

if exist "AIBandPlugin.jucer" (
    echo ✅ AIBandPlugin.jucer found
) else (
    echo ❌ AIBandPlugin.jucer missing
)

echo.
echo Checking for syntax issues...
echo Note: This is a basic check - full compilation requires JUCE and build tools

:: Check for common C++ syntax patterns
findstr /C:"#include" Source\*.cpp Source\*.h >nul
if %errorlevel% equ 0 (
    echo ✅ Include statements found in source files
) else (
    echo ⚠️  No include statements found
)

findstr /C:"class " Source\*.h >nul
if %errorlevel% equ 0 (
    echo ✅ Class declarations found
) else (
    echo ⚠️  No class declarations found
)

findstr /C:"void processBlock" Source\PluginProcessor.cpp >nul
if %errorlevel% equ 0 (
    echo ✅ Audio processing method found
) else (
    echo ⚠️  Audio processing method not found
)

findstr /C:"assertEqual" Tests\*.cpp >nul
if %errorlevel% equ 0 (
    echo ❌ Old assertEqual calls found - should use typed assertions
) else (
    echo ✅ Test assertions properly updated
)

echo.
echo ===========================================
echo Project Validation Complete!
echo ===========================================
echo.
echo Next steps:
echo 1. Install JUCE Framework from https://juce.com/get-juce
echo 2. Open AIBandPlugin.jucer in Projucer
echo 3. Set JUCE modules path and generate project files
echo 4. Build the project in Visual Studio
echo 5. Run tests to verify functionality
echo.
echo For detailed setup instructions, see SETUP.md
echo.

pause
