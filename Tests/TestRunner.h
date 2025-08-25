#pragma once

#include <JuceHeader.h>
#include "TestFramework.h"
#include "MidiManagerTests.h"
#include "PluginProcessorTests.h"

//==============================================================================
/**
    Main Test Runner for AI Band Plugin
    
    Runs all test suites and provides comprehensive testing of the plugin.
    This can be run as a standalone console application or integrated into
    the plugin for runtime testing.
*/
class TestRunner
{
public:
    //==============================================================================
    TestRunner();
    ~TestRunner();
    
    //==============================================================================
    /** Run all test suites */
    static bool runAllTests();
    
    /** Run specific test suite */
    static bool runTestSuite(const juce::String& suiteName);
    
    /** Get list of available test suites */
    static juce::StringArray getAvailableTestSuites();
    
    /** Run tests and generate report */
    static juce::String runTestsWithReport();

private:
    //==============================================================================
    /** Run individual test suites */
    static bool runMidiManagerTests();
    static bool runPluginProcessorTests();
    static bool runIntegrationTests();
    
    /** Run integration tests that test component interaction */
    static bool testMidiManagerIntegration();
    static bool testPluginProcessorIntegration();
    static bool testEndToEndWorkflow();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestRunner)
};
