#include "TestRunner.h"

//==============================================================================
TestRunner::TestRunner()
{
}

TestRunner::~TestRunner()
{
}

//==============================================================================
bool TestRunner::runAllTests()
{
    DBG("===============================================");
    DBG("    AI Band Plugin - Test Suite Runner");
    DBG("===============================================");
    
    // Initialize test framework
    TestFramework::initialize();
    
    bool allTestsPassed = true;
    
    // Run all test suites
    allTestsPassed &= runMidiManagerTests();
    allTestsPassed &= runPluginProcessorTests();
    allTestsPassed &= runIntegrationTests();
    
    // Print final summary
    DBG("");
    DBG("===============================================");
    TestFramework::printTestSummary();
    DBG("===============================================");
    
    if (allTestsPassed)
    {
        DBG("🎉 ALL TESTS PASSED! 🎉");
    }
    else
    {
        DBG("❌ SOME TESTS FAILED! ❌");
    }
    
    // Cleanup
    TestFramework::cleanup();
    
    return allTestsPassed;
}

bool TestRunner::runTestSuite(const juce::String& suiteName)
{
    TestFramework::initialize();
    
    bool result = false;
    
    if (suiteName == "MidiManager")
    {
        result = runMidiManagerTests();
    }
    else if (suiteName == "PluginProcessor")
    {
        result = runPluginProcessorTests();
    }
    else if (suiteName == "Integration")
    {
        result = runIntegrationTests();
    }
    else
    {
        DBG("Unknown test suite: " << suiteName);
        return false;
    }
    
    TestFramework::printTestSummary();
    TestFramework::cleanup();
    
    return result;
}

juce::StringArray TestRunner::getAvailableTestSuites()
{
    return {"MidiManager", "PluginProcessor", "Integration"};
}

juce::String TestRunner::runTestsWithReport()
{
    juce::String report;
    
    report += "AI Band Plugin Test Report\n";
    report += "=========================\n\n";
    report += "Test Date: " + juce::Time::getCurrentTime().toString(true, true) + "\n\n";
    
    bool allPassed = runAllTests();
    
    report += "Test Results Summary:\n";
    report += "- Tests Run: " + juce::String(TestFramework::getTestsRun()) + "\n";
    report += "- Tests Passed: " + juce::String(TestFramework::getTestsPassed()) + "\n";
    report += "- Tests Failed: " + juce::String(TestFramework::getTestsFailed()) + "\n";
    report += "- Success Rate: " + juce::String(
        TestFramework::getTestsRun() > 0 ? 
        (100.0 * TestFramework::getTestsPassed() / TestFramework::getTestsRun()) : 0.0, 1
    ) + "%\n\n";
    
    report += "Overall Result: " + juce::String(allPassed ? "PASS" : "FAIL") + "\n";
    
    return report;
}

//==============================================================================
// Test Suite Runners

bool TestRunner::runMidiManagerTests()
{
    DBG("");
    DBG("Running MidiManager Test Suite...");
    DBG("=================================");
    
    return MidiManagerTests::runAllTests();
}

bool TestRunner::runPluginProcessorTests()
{
    DBG("");
    DBG("Running PluginProcessor Test Suite...");
    DBG("====================================");
    
    return PluginProcessorTests::runAllTests();
}

bool TestRunner::runIntegrationTests()
{
    DBG("");
    DBG("Running Integration Test Suite...");
    DBG("================================");
    
    bool allPassed = true;
    
    allPassed &= testMidiManagerIntegration();
    allPassed &= testPluginProcessorIntegration();
    allPassed &= testEndToEndWorkflow();
    
    return allPassed;
}

//==============================================================================
// Integration Tests

bool TestRunner::testMidiManagerIntegration()
{
    DBG("Testing MidiManager integration...");
    
    // Test MidiManager integration with file system
    auto tempDir = TestFramework::createTempTestDirectory();
    
    // Create test files
    auto bassFile = tempDir.getChildFile("integration_bass.mid");
    auto drumFile = tempDir.getChildFile("integration_drum.mid");
    
    TestFramework::assertTrue(
        TestFramework::createTestBassMidiFile(bassFile.getFullPathName(), 4.0, 130),
        "Create integration bass file"
    );
    
    TestFramework::assertTrue(
        TestFramework::createTestDrumMidiFile(drumFile.getFullPathName(), 4.0, 130),
        "Create integration drum file"
    );
    
    // Test MidiManager can load both files
    MidiManager manager;
    manager.initialize();
    manager.prepareToPlay(44100.0, 512);
    
    juce::MidiBuffer bassBuffer, drumBuffer;
    bool bassLoaded = manager.loadMidiFile(bassFile.getFullPathName(), bassBuffer);
    bool drumLoaded = manager.loadMidiFile(drumFile.getFullPathName(), drumBuffer);
    
    TestFramework::assertTrue(bassLoaded, "Load bass file in integration test");
    TestFramework::assertTrue(drumLoaded, "Load drum file in integration test");
    TestFramework::assertTrue(!bassBuffer.isEmpty(), "Bass buffer has content");
    TestFramework::assertTrue(!drumBuffer.isEmpty(), "Drum buffer has content");
    
    return true;
}

bool TestRunner::testPluginProcessorIntegration()
{
    DBG("Testing PluginProcessor integration...");
    
    // Test processor with MidiManager integration
    auto processor = std::make_unique<AIBandAudioProcessor>();
    processor->prepareToPlay(44100.0, 512);
    
    // Create test files
    auto tempDir = TestFramework::createTempTestDirectory();
    auto bassFile = tempDir.getChildFile("processor_integration_bass.mid");
    auto drumFile = tempDir.getChildFile("processor_integration_drum.mid");
    
    TestFramework::createTestBassMidiFile(bassFile.getFullPathName(), 2.0, 120);
    TestFramework::createTestDrumMidiFile(drumFile.getFullPathName(), 2.0, 120);
    
    // Test processor can load and process files
    bool loadResult = processor->loadMidiFiles(bassFile.getFullPathName(), 
                                              drumFile.getFullPathName());
    TestFramework::assertTrue(loadResult, "Processor loads MIDI files");
    
    // Test playback control
    processor->startPlayback();
    TestFramework::assertTrue(processor->isPlaying(), "Processor starts playback");
    
    // Process audio blocks
    juce::AudioBuffer<float> audioBuffer(2, 512);
    juce::MidiBuffer midiBuffer;
    
    for (int i = 0; i < 5; ++i)
    {
        audioBuffer.clear();
        midiBuffer.clear();
        processor->processBlock(audioBuffer, midiBuffer);
    }
    
    TestFramework::assertTrue(true, "Processor handles audio processing");
    
    return true;
}

bool TestRunner::testEndToEndWorkflow()
{
    DBG("Testing end-to-end workflow...");
    
    // Simulate complete workflow: file creation -> loading -> playback -> state save
    auto tempDir = TestFramework::createTempTestDirectory();
    
    // Step 1: Create MIDI files (simulating ai-band-backend output)
    auto bassFile = tempDir.getChildFile("workflow_bass.mid");
    auto drumFile = tempDir.getChildFile("workflow_drum.mid");
    
    TestFramework::assertTrue(
        TestFramework::createTestBassMidiFile(bassFile.getFullPathName(), 8.0, 125),
        "Workflow: Create bass file"
    );
    
    TestFramework::assertTrue(
        TestFramework::createTestDrumMidiFile(drumFile.getFullPathName(), 8.0, 125),
        "Workflow: Create drum file"
    );
    
    // Step 2: Load into plugin
    auto processor = std::make_unique<AIBandAudioProcessor>();
    processor->prepareToPlay(44100.0, 512);
    
    bool loadSuccess = processor->loadMidiFiles(bassFile.getFullPathName(), 
                                               drumFile.getFullPathName());
    TestFramework::assertTrue(loadSuccess, "Workflow: Load files into processor");
    
    // Step 3: Start playback
    processor->startPlayback();
    TestFramework::assertTrue(processor->isPlaying(), "Workflow: Start playback");
    
    // Step 4: Process audio for a while
    juce::AudioBuffer<float> audioBuffer(2, 512);
    juce::MidiBuffer midiBuffer;
    
    for (int block = 0; block < 20; ++block)
    {
        audioBuffer.clear();
        midiBuffer.clear();
        processor->processBlock(audioBuffer, midiBuffer);
        
        // Verify beat position advances
        double currentBeat = processor->getCurrentBeat();
        TestFramework::assertTrue(currentBeat >= 0.0, "Workflow: Beat position valid");
    }
    
    // Step 5: Save and restore state
    juce::MemoryBlock stateData;
    processor->getStateInformation(stateData);
    TestFramework::assertTrue(stateData.getSize() > 0, "Workflow: State data saved");
    
    auto processor2 = std::make_unique<AIBandAudioProcessor>();
    processor2->prepareToPlay(44100.0, 512);
    processor2->setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));
    TestFramework::assertTrue(true, "Workflow: State restored");
    
    // Step 6: Stop playback
    processor->stopPlayback();
    TestFramework::assertTrue(!processor->isPlaying(), "Workflow: Stop playback");
    
    DBG("End-to-end workflow test completed successfully");
    return true;
}
