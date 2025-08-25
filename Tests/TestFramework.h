#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
    Test Framework for AI Band Plugin
    
    This class provides utilities for testing the AI Band Plugin components.
    It includes MIDI generation, file system mocking, and assertion helpers.
*/
class TestFramework
{
public:
    //==============================================================================
    TestFramework();
    ~TestFramework();
    
    //==============================================================================
    // Test Environment Setup
    
    /** Initialize test environment */
    static bool initialize();
    
    /** Cleanup test environment */
    static void cleanup();
    
    /** Create temporary test directory */
    static juce::File createTempTestDirectory();
    
    /** Remove temporary test files */
    static void cleanupTempFiles();
    
    //==============================================================================
    // MIDI Test Utilities
    
    /** Create a simple test MIDI file with bass notes */
    static bool createTestBassMidiFile(const juce::String& filePath, 
                                      double durationInBeats = 8.0,
                                      int tempo = 120);
    
    /** Create a simple test MIDI file with drum pattern */
    static bool createTestDrumMidiFile(const juce::String& filePath,
                                      double durationInBeats = 8.0,
                                      int tempo = 120);
    
    /** Create an invalid MIDI file for error testing */
    static bool createInvalidMidiFile(const juce::String& filePath);
    
    /** Validate MIDI buffer contents */
    static bool validateMidiBuffer(const juce::MidiBuffer& buffer,
                                  int expectedEventCount = -1,
                                  double expectedDuration = -1.0);
    
    //==============================================================================
    // Audio Test Utilities
    
    /** Create test audio buffer with silence */
    static juce::AudioBuffer<float> createSilentAudioBuffer(int numChannels, int numSamples);
    
    /** Create test MIDI buffer with simple pattern */
    static juce::MidiBuffer createTestMidiBuffer(double durationInBeats = 4.0, int tempo = 120);
    
    /** Validate audio processing (check for silence, clipping, etc.) */
    static bool validateAudioOutput(const juce::AudioBuffer<float>& buffer,
                                   bool expectSilence = true);
    
    //==============================================================================
    // Assertion Helpers
    
    /** Assert that condition is true */
    static void assertTrue(bool condition, const juce::String& message);
    
    /** Assert that integer values are equal */
    static void assertEqualInt(int expected, int actual, const juce::String& message);
    
    /** Assert that double values are equal */
    static void assertEqualDouble(double expected, double actual, const juce::String& message);
    
    /** Assert that string values are equal */
    static void assertEqualString(const juce::String& expected, const juce::String& actual, const juce::String& message);
    
    /** Assert that floating point values are approximately equal */
    static void assertApproxEqual(double expected, double actual, 
                                 double tolerance, const juce::String& message);
    
    /** Assert that file exists */
    static void assertFileExists(const juce::String& filePath, const juce::String& message);
    
    //==============================================================================
    // Test Result Tracking
    
    /** Get number of tests run */
    static int getTestsRun() { return testsRun; }
    
    /** Get number of tests passed */
    static int getTestsPassed() { return testsPassed; }
    
    /** Get number of tests failed */
    static int getTestsFailed() { return testsFailed; }
    
    /** Reset test counters */
    static void resetCounters();
    
    /** Print test summary */
    static void printTestSummary();

private:
    //==============================================================================
    static int testsRun;
    static int testsPassed;
    static int testsFailed;
    static juce::Array<juce::File> tempFilesToCleanup;
    
    //==============================================================================
    static void incrementTestCount();
    static void incrementPassCount();
    static void incrementFailCount();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestFramework)
};
