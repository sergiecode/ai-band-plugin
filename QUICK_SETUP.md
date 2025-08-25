# AI Band Plugin - Quick Setup Guide

## Fix VS Code Include Errors

### Step 1: Install Development Tools

**Install Visual Studio Community (Free):**
1. Go to https://visualstudio.microsoft.com/downloads/
2. Download Visual Studio Community 2022
3. During installation, select "Desktop development with C++"
4. This installs the Microsoft C++ compiler and standard library

**Install JUCE Framework:**
1. Go to https://juce.com/get-juce
2. Download and install to `C:\JUCE\`
3. This provides the Projucer and all JUCE modules

### Step 2: Configure VS Code

**Install C++ Extension:**
1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Install "C/C++" by Microsoft

**Configure IntelliSense:**
1. Press Ctrl+Shift+P
2. Type "C/C++: Select IntelliSense Configuration"
3. Choose your Visual Studio installation
4. This fixes the include errors!

### Step 3: Update Include Paths

Edit `.vscode/c_cpp_properties.json` and update the JUCE path:

```json
{
    "configurations": [
        {
            "name": "AI Band Plugin",
            "includePath": [
                "${workspaceFolder}/Source/**",
                "${workspaceFolder}/Tests/**",
                "C:/JUCE/modules/**"
            ],
            "defines": [
                "JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1",
                "WIN32", "_WIN32", "_WINDOWS"
            ],
            "compilerPath": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/*/bin/Hostx64/x64/cl.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-msvc-x64"
        }
    ],
    "version": 4
}
```

## How to Build and Test

### Method 1: Using Projucer (Easiest)

1. **Open Project:**
   ```
   Open AIBandPlugin.jucer in Projucer
   ```

2. **Configure JUCE Path:**
   - In Projucer, go to Settings
   - Set "Path to JUCE" to your JUCE installation folder
   - Click "Save Project"

3. **Generate Visual Studio Project:**
   - Projucer will create Builds/VisualStudio2022/ folder
   - Open the .sln file in Visual Studio

4. **Build:**
   - Press Ctrl+Shift+B in Visual Studio
   - Choose Release or Debug configuration

### Method 2: Using CMake

1. **Install CMake:**
   ```
   Download from: https://cmake.org/download/
   Add to PATH during installation
   ```

2. **Build:**
   ```cmd
   mkdir build
   cd build
   cmake .. -DBUILD_TESTS=ON
   cmake --build . --config Release
   ```

### Method 3: Test Core Logic Only

Run our simple test without JUCE:

1. **Option A - Online Compiler:**
   - Copy simple_test.cpp contents
   - Paste at https://onlinegdb.com/online_c++_compiler
   - Click Run

2. **Option B - Install MinGW:**
   ```cmd
   # Download MinGW from mingw-w64.org
   # Add to PATH, then:
   g++ -o simple_test simple_test.cpp
   simple_test.exe
   ```

## Testing the Complete Plugin

Once built, you can test the plugin:

### Unit Tests
```cmd
# After building with CMake:
.\run_tests.bat

# Or run the test executable directly:
build\Debug\AIBandPluginTests.exe
```

### Integration Test
```cmd
# Test compatibility with ai-band-backend:
python test_integration.py
```

### DAW Testing
1. Copy the built plugin to your DAW's plugin folder:
   - VST3: `C:\Program Files\Common Files\VST3\`
   - VST2: Your DAW's VST folder

2. Load the plugin in your DAW

3. Test with MIDI files from ai-band-backend

## Troubleshooting

**VS Code still shows errors after setup:**
- Restart VS Code
- Press Ctrl+Shift+P → "C/C++: Reset IntelliSense Database"
- Check that compiler path in c_cpp_properties.json is correct

**Build fails:**
- Verify JUCE installation path
- Check that Visual Studio C++ tools are installed
- Make sure you have the correct JUCE version (7.0+)

**Plugin doesn't load in DAW:**
- Build in Release mode for distribution
- Check DAW plugin scan paths
- Verify plugin format (VST3 is recommended)

## Quick Verification

To quickly verify everything is working:

1. **Test core logic:** Run simple_test.cpp online
2. **Check files:** Run verify_project.bat
3. **Test integration:** Run test_integration.py
4. **Build plugin:** Use Projucer + Visual Studio
