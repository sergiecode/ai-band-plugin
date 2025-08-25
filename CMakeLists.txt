cmake_minimum_required(VERSION 3.22)

project(AIBandPlugin VERSION 1.0.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE subdirectory (assumes JUCE is installed or included as submodule)
# Option 1: If JUCE is installed system-wide
find_package(JUCE CONFIG REQUIRED)

# Option 2: If JUCE is included as git submodule (uncomment if needed)
# add_subdirectory(JUCE)

# Plugin configuration
juce_add_plugin(AIBandPlugin
    # Plugin identification
    COMPANY_NAME "AIBand"
    PLUGIN_MANUFACTURER_CODE "AiBd"
    PLUGIN_CODE "AiBP"
    
    # Plugin formats
    FORMATS AU VST3 Standalone
    
    # Plugin type
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    
    # Product info
    PRODUCT_NAME "AI Band Plugin"
    DESCRIPTION "AI-powered bass and drum accompaniment plugin"
    
    # Bundle info
    BUNDLE_ID com.aiband.plugin
)

# Source files
target_sources(AIBandPlugin
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/MidiManager.cpp
        Source/MidiManager.h
        Source/NetworkClient.cpp
        Source/NetworkClient.h
)

# Include directories
target_include_directories(AIBandPlugin
    PRIVATE
        Source
)

# Link JUCE libraries
target_link_libraries(AIBandPlugin
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Preprocessor definitions
target_compile_definitions(AIBandPlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_ALSA=1
        JUCE_DIRECTSOUND=1
        JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(AIBandPlugin PRIVATE JUCE_WINDOWS=1)
elseif(APPLE)
    target_compile_definitions(AIBandPlugin PRIVATE JUCE_MAC=1)
elseif(UNIX)
    target_compile_definitions(AIBandPlugin PRIVATE JUCE_LINUX=1)
endif()

# Copy plugin to common locations after build (optional)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Add custom commands to install plugin to standard locations
    # This will be platform-specific
endif()

#==============================================================================
# Test Application (Optional)
option(BUILD_TESTS "Build test application" OFF)

if(BUILD_TESTS)
    # Create test executable
    juce_add_console_app(AIBandPluginTests
        PRODUCT_NAME "AI Band Plugin Tests"
    )
    
    # Test source files
    target_sources(AIBandPluginTests
        PRIVATE
            Tests/TestApplication.cpp
            Tests/TestRunner.cpp
            Tests/TestRunner.h
            Tests/TestFramework.cpp
            Tests/TestFramework.h
            Tests/MidiManagerTests.cpp
            Tests/MidiManagerTests.h
            Tests/PluginProcessorTests.cpp
            Tests/PluginProcessorTests.h
            
            # Include source files for testing
            Source/MidiManager.cpp
            Source/MidiManager.h
            Source/NetworkClient.cpp
            Source/NetworkClient.h
            Source/PluginProcessor.cpp
            Source/PluginProcessor.h
    )
    
    # Include directories for tests
    target_include_directories(AIBandPluginTests
        PRIVATE
            Source
            Tests
    )
    
    # Link JUCE libraries for tests
    target_link_libraries(AIBandPluginTests
        PRIVATE
            juce::juce_audio_basics
            juce::juce_audio_devices
            juce::juce_audio_formats
            juce::juce_audio_processors
            juce::juce_audio_utils
            juce::juce_core
            juce::juce_data_structures
            juce::juce_events
            juce::juce_graphics
            juce::juce_gui_basics
            juce::juce_gui_extra
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags
    )
    
    # Test-specific compile definitions
    target_compile_definitions(AIBandPluginTests
        PUBLIC
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_VST3_CAN_REPLACE_VST2=0
            JUCE_DISPLAY_SPLASH_SCREEN=0
            JUCE_REPORT_APP_USAGE=0
            JUCE_ALSA=1
            JUCE_DIRECTSOUND=1
            JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
            JUCE_UNIT_TESTS=1
    )
    
    # Platform-specific settings for tests
    if(WIN32)
        target_compile_definitions(AIBandPluginTests PRIVATE JUCE_WINDOWS=1)
    elseif(APPLE)
        target_compile_definitions(AIBandPluginTests PRIVATE JUCE_MAC=1)
    elseif(UNIX)
        target_compile_definitions(AIBandPluginTests PRIVATE JUCE_LINUX=1)
    endif()
endif()
