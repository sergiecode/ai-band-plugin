# AI Band Plugin - Development Setup Guide

This guide will help you set up the development environment to build and test the AI Band Plugin.

## Prerequisites

### Required Tools
1. **JUCE Framework** (version 7.0 or later)
   - Download from: https://juce.com/get-juce
   - Install the Projucer application

2. **Visual Studio Community** (2019 or later) OR **Visual Studio Code** with C++ extensions
   - Download from: https://visualstudio.microsoft.com/downloads/
   - Make sure to install the "Desktop development with C++" workload

3. **CMake** (version 3.15 or later) - Optional but recommended
   - Download from: https://cmake.org/download/
   - Add to PATH during installation

## Quick Start (Using Projucer)

### Option 1: Using Projucer (Easiest)
1. Open the Projucer application
2. Click "Open Project" and select `AIBandPlugin.jucer`
3. Set your JUCE modules path in the project settings
4. Click "Save Project" to generate Visual Studio project files
5. Open the generated `.sln` file in Visual Studio
6. Build the project using Ctrl+Shift+B

### Option 2: Using CMake (Advanced)
1. Open Command Prompt or PowerShell
2. Navigate to the plugin directory:
   ```cmd
   cd "C:\Users\SnS_D\Desktop\IA\ai-band-plugin"
   ```
3. Create a build directory:
   ```cmd
   mkdir build
   cd build
   ```
4. Configure the project:
   ```cmd
   cmake .. -DBUILD_TESTS=ON
   ```
5. Build the project:
   ```cmd
   cmake --build . --config Release
   ```

## Running Tests

After building successfully:

### If using CMake:
```cmd
# From the plugin root directory
.\run_tests.bat
```

### If using Visual Studio:
1. Set the test project as startup project
2. Press F5 or Ctrl+F5 to run tests

## Project Structure

```
ai-band-plugin/
├── Source/                 # Main plugin source code
│   ├── PluginProcessor.cpp # Audio processing logic
│   ├── PluginEditor.cpp    # User interface
│   ├── MidiManager.cpp     # MIDI file handling
│   └── NetworkClient.cpp   # Backend communication
├── Tests/                  # Test suite
│   ├── TestFramework.cpp   # Testing utilities
│   ├── MidiManagerTests.cpp# MIDI functionality tests
│   └── PluginProcessorTests.cpp # Audio processing tests
├── CMakeLists.txt          # Build configuration
└── AIBandPlugin.jucer      # JUCE project file
```

## Testing the Plugin

### Unit Tests
The plugin includes comprehensive unit tests for:
- MIDI file loading and processing
- Audio processing pipeline
- UI state management
- Integration with ai-band-backend

### Integration Testing
To test with the AI Band Backend:
1. Ensure the ai-band-backend is running
2. Place generated MIDI files in the monitored folder
3. Load the plugin in your DAW
4. Verify automatic playback of AI-generated content

## Troubleshooting

### Common Issues:
1. **CMake not found**: Install CMake and add to PATH
2. **JUCE modules not found**: Set correct JUCE path in Projucer
3. **Compiler errors**: Ensure Visual Studio C++ tools are installed
4. **Plugin not loading in DAW**: Build in Release mode for distribution

### Build Verification:
After successful build, you should find:
- `AIBandPlugin.vst3` (VST3 plugin)
- `AIBandPlugin.component` (AU plugin on macOS)
- Test executables in build directory

## Development Workflow

1. **Edit Code**: Use Visual Studio, VS Code, or any C++ IDE
2. **Build**: Use either CMake or Projucer-generated projects
3. **Test**: Run unit tests with `run_tests.bat`
4. **Debug**: Load plugin in DAW or use standalone mode
5. **Integration**: Test with ai-band-backend MIDI generation

## Next Steps

Once the environment is set up:
1. Build the plugin using your preferred method
2. Run the test suite to verify functionality
3. Test integration with the ai-band-backend
4. Load the plugin in your DAW for real-world testing

For detailed API documentation, see the inline comments in the source files.
