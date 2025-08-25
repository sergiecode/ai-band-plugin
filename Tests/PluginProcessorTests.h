#pragma once

#include <JuceHeader.h>
#include "TestFramework.h"
#include "../Source/PluginProcessor.h"

//==============================================================================
/**
    Unit Tests for AIBandAudioProcessor class
    
    Tests the core plugin functionality including:
    - Audio processing
    - MIDI handling
    - File loading
    - Playback control
    - State management
*/
class PluginProcessorTests
{
public:
    //==============================================================================
    PluginProcessorTests();
    ~PluginProcessorTests();
    
    //==============================================================================
    /** Run all PluginProcessor tests */
    static bool runAllTests();
    
    //==============================================================================
    // Individual Test Methods
    
    /** Test plugin initialization and basic setup */
    static bool testPluginInitialization();
    
    /** Test audio processing with silent input */
    static bool testAudioProcessing();
    
    /** Test MIDI file loading functionality */
    static bool testMidiFileLoading();
    
    /** Test playback control (start/stop) */
    static bool testPlaybackControl();
    
    /** Test folder monitoring for new files */
    static bool testFolderMonitoring();
    
    /** Test beat position tracking */
    static bool testBeatPositionTracking();
    
    /** Test state save and restore */
    static bool testStateManagement();
    
    /** Test plugin metadata and properties */
    static bool testPluginMetadata();
    
    /** Test bus layout validation */
    static bool testBusLayouts();
    
    /** Test MIDI event processing */
    static bool testMidiEventProcessing();

private:
    //==============================================================================
    /** Helper method to create test processor instance */
    static std::unique_ptr<AIBandAudioProcessor> createTestProcessor();
    
    /** Helper method to prepare processor for testing */
    static void prepareProcessor(AIBandAudioProcessor& processor);
    
    /** Helper method to create test audio and MIDI buffers */
    static void createTestBuffers(juce::AudioBuffer<float>& audioBuffer, 
                                 juce::MidiBuffer& midiBuffer,
                                 int numSamples = 512);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessorTests)
};
