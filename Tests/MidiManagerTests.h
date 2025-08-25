#pragma once

#include <JuceHeader.h>
#include "TestFramework.h"
#include "../Source/MidiManager.h"

//==============================================================================
/**
    Unit Tests for MidiManager class
    
    Tests all functionality of the MidiManager including:
    - MIDI file loading and saving
    - Format validation
    - Tempo and time signature detection
    - Buffer management
    - Error handling
*/
class MidiManagerTests
{
public:
    //==============================================================================
    MidiManagerTests();
    ~MidiManagerTests();
    
    //==============================================================================
    /** Run all MidiManager tests */
    static bool runAllTests();
    
    //==============================================================================
    // Individual Test Methods
    
    /** Test MIDI file loading with valid files */
    static bool testMidiFileLoading();
    
    /** Test MIDI file loading with invalid files */
    static bool testInvalidMidiFileHandling();
    
    /** Test MIDI file saving functionality */
    static bool testMidiFileSaving();
    
    /** Test tempo detection from MIDI files */
    static bool testTempoDetection();
    
    /** Test time signature detection */
    static bool testTimeSignatureDetection();
    
    /** Test MIDI duration calculation */
    static bool testDurationCalculation();
    
    /** Test beat/sample conversion utilities */
    static bool testBeatSampleConversion();
    
    /** Test file format validation */
    static bool testFileFormatValidation();
    
    /** Test MidiManager initialization and cleanup */
    static bool testInitializationAndCleanup();
    
    /** Test error handling with corrupt files */
    static bool testErrorHandling();

private:
    //==============================================================================
    /** Helper method to create test MidiManager instance */
    static std::unique_ptr<MidiManager> createTestMidiManager();
    
    /** Helper method to validate MIDI buffer contents */
    static bool validateMidiBufferContents(const juce::MidiBuffer& buffer, 
                                          const juce::String& expectedContent);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiManagerTests)
};
