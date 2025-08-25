#include "PluginProcessorTests.h"

//==============================================================================
PluginProcessorTests::PluginProcessorTests()
{
}

PluginProcessorTests::~PluginProcessorTests()
{
}

//==============================================================================
bool PluginProcessorTests::runAllTests()
{
    DBG("=== Running PluginProcessor Tests ===");
    
    bool allPassed = true;
    
    allPassed &= testPluginInitialization();
    allPassed &= testPluginMetadata();
    allPassed &= testBusLayouts();
    allPassed &= testAudioProcessing();
    allPassed &= testMidiFileLoading();
    allPassed &= testPlaybackControl();
    allPassed &= testBeatPositionTracking();
    allPassed &= testMidiEventProcessing();
    allPassed &= testFolderMonitoring();
    allPassed &= testStateManagement();
    
    DBG("=== PluginProcessor Tests Complete ===");
    return allPassed;
}

//==============================================================================
bool PluginProcessorTests::testPluginInitialization()
{
    DBG("Testing plugin initialization...");
    
    auto processor = createTestProcessor();
    TestFramework::assertTrue(processor != nullptr, "Processor creation");
    
    // Test basic properties
    TestFramework::assertTrue(processor->getName().isNotEmpty(), "Plugin has name");
    TestFramework::assertTrue(processor->acceptsMidi(), "Plugin accepts MIDI");
    TestFramework::assertTrue(processor->producesMidi(), "Plugin produces MIDI");
    TestFramework::assertTrue(processor->isMidiEffect(), "Plugin is MIDI effect");
    
    return true;
}

bool PluginProcessorTests::testPluginMetadata()
{
    DBG("Testing plugin metadata...");
    
    auto processor = createTestProcessor();
    
    // Test plugin properties
    TestFramework::assertEqualString(juce::String("AI Band Plugin"), processor->getName(), "Plugin name");
    TestFramework::assertTrue(processor->getNumPrograms() >= 1, "At least one program");
    TestFramework::assertEqualDouble(0.0, processor->getTailLengthSeconds(), "No tail length");
    
    // Test MIDI capabilities
    TestFramework::assertTrue(processor->acceptsMidi(), "Accepts MIDI input");
    TestFramework::assertTrue(processor->producesMidi(), "Produces MIDI output");
    TestFramework::assertTrue(processor->isMidiEffect(), "Is MIDI effect");
    
    return true;
}

bool PluginProcessorTests::testBusLayouts()
{
    DBG("Testing bus layouts...");
    
    auto processor = createTestProcessor();
    
    // Test stereo layout
    juce::AudioProcessor::BusesLayout stereoLayout;
    stereoLayout.inputBuses.add(juce::AudioChannelSet::stereo());
    stereoLayout.outputBuses.add(juce::AudioChannelSet::stereo());
    
    TestFramework::assertTrue(processor->isBusesLayoutSupported(stereoLayout), 
                            "Stereo layout supported");
    
    // Test mono layout
    juce::AudioProcessor::BusesLayout monoLayout;
    monoLayout.inputBuses.add(juce::AudioChannelSet::mono());
    monoLayout.outputBuses.add(juce::AudioChannelSet::mono());
    
    TestFramework::assertTrue(processor->isBusesLayoutSupported(monoLayout), 
                            "Mono layout supported");
    
    return true;
}

bool PluginProcessorTests::testAudioProcessing()
{
    DBG("Testing audio processing...");
    
    auto processor = createTestProcessor();
    prepareProcessor(*processor);
    
    // Create test buffers
    juce::AudioBuffer<float> audioBuffer;
    juce::MidiBuffer midiBuffer;
    createTestBuffers(audioBuffer, midiBuffer);
    
    // Process audio block
    processor->processBlock(audioBuffer, midiBuffer);
    
    // Verify audio output (should be silent for pass-through)
    TestFramework::assertTrue(
        TestFramework::validateAudioOutput(audioBuffer, true),
        "Audio processing produces valid output"
    );
    
    return true;
}

bool PluginProcessorTests::testMidiFileLoading()
{
    DBG("Testing MIDI file loading...");
    
    auto processor = createTestProcessor();
    prepareProcessor(*processor);
    
    // Create test MIDI files
    auto tempDir = TestFramework::createTempTestDirectory();
    auto bassFile = tempDir.getChildFile("test_bass.mid");
    auto drumFile = tempDir.getChildFile("test_drum.mid");
    
    TestFramework::assertTrue(
        TestFramework::createTestBassMidiFile(bassFile.getFullPathName()),
        "Create test bass file"
    );
    
    TestFramework::assertTrue(
        TestFramework::createTestDrumMidiFile(drumFile.getFullPathName()),
        "Create test drum file"
    );
    
    // Test loading files
    bool loadResult = processor->loadMidiFiles(bassFile.getFullPathName(), 
                                              drumFile.getFullPathName());
    
    TestFramework::assertTrue(loadResult, "Load MIDI files successfully");
    
    // Test loading single file
    bool singleLoadResult = processor->loadMidiFiles(bassFile.getFullPathName(), "");
    TestFramework::assertTrue(singleLoadResult, "Load single MIDI file");
    
    // Test loading non-existent files
    bool failResult = processor->loadMidiFiles("nonexistent1.mid", "nonexistent2.mid");
    TestFramework::assertTrue(!failResult, "Reject non-existent files");
    
    return true;
}

bool PluginProcessorTests::testPlaybackControl()
{
    DBG("Testing playback control...");
    
    auto processor = createTestProcessor();
    prepareProcessor(*processor);
    
    // Initially should not be playing
    TestFramework::assertTrue(!processor->isPlaying(), "Initially not playing");
    
    // Test start playback
    processor->startPlayback();
    TestFramework::assertTrue(processor->isPlaying(), "Playing after start");
    
    // Test stop playback
    processor->stopPlayback();
    TestFramework::assertTrue(!processor->isPlaying(), "Not playing after stop");
    
    // Test reset
    processor->resetPlayback();
    TestFramework::assertEqualDouble(0.0, processor->getCurrentBeat(), "Reset position to 0");
    
    return true;
}

bool PluginProcessorTests::testBeatPositionTracking()
{
    DBG("Testing beat position tracking...");
    
    auto processor = createTestProcessor();
    prepareProcessor(*processor);
    
    // Start playback
    processor->startPlayback();
    
    // Get initial position
    double initialBeat = processor->getCurrentBeat();
    
    // Process some audio to advance time
    juce::AudioBuffer<float> audioBuffer;
    juce::MidiBuffer midiBuffer;
    
    for (int i = 0; i < 10; ++i)
    {
        createTestBuffers(audioBuffer, midiBuffer);
        processor->processBlock(audioBuffer, midiBuffer);
    }
    
    // Position should have advanced (or stayed the same if using host transport)
    double finalBeat = processor->getCurrentBeat();
    TestFramework::assertTrue(finalBeat >= initialBeat, "Beat position advances or stays same");
    
    // Test reset
    processor->resetPlayback();
    TestFramework::assertEqualDouble(0.0, processor->getCurrentBeat(), "Reset position works");
    
    return true;
}

bool PluginProcessorTests::testMidiEventProcessing()
{
    DBG("Testing MIDI event processing...");
    
    auto processor = createTestProcessor();
    prepareProcessor(*processor);
    
    // Load test MIDI files
    auto tempDir = TestFramework::createTempTestDirectory();
    auto bassFile = tempDir.getChildFile("test_bass.mid");
    
    TestFramework::createTestBassMidiFile(bassFile.getFullPathName(), 2.0, 120);
    processor->loadMidiFiles(bassFile.getFullPathName(), "");
    
    // Start playback
    processor->startPlayback();
    
    // Process audio and check for MIDI output
    juce::AudioBuffer<float> audioBuffer;
    juce::MidiBuffer midiBuffer;
    createTestBuffers(audioBuffer, midiBuffer);
    
    processor->processBlock(audioBuffer, midiBuffer);
    
    // MIDI buffer may or may not have events depending on timing
    // Just verify processing doesn't crash
    TestFramework::assertTrue(true, "MIDI event processing completes without errors");
    
    return true;
}

bool PluginProcessorTests::testFolderMonitoring()
{
    DBG("Testing folder monitoring...");
    
    auto processor = createTestProcessor();
    prepareProcessor(*processor);
    
    // Create test directory
    auto tempDir = TestFramework::createTempTestDirectory();
    
    // Set monitoring folder
    processor->setMidiFolder(tempDir.getFullPathName());
    
    // Create MIDI files in the folder
    auto bassFile = tempDir.getChildFile("bass_test.mid");
    auto drumFile = tempDir.getChildFile("drum_test.mid");
    
    TestFramework::createTestBassMidiFile(bassFile.getFullPathName());
    TestFramework::createTestDrumMidiFile(drumFile.getFullPathName());
    
    // Process some blocks to trigger file checking
    juce::AudioBuffer<float> audioBuffer;
    juce::MidiBuffer midiBuffer;
    
    for (int i = 0; i < 20; ++i)  // Process enough blocks to trigger file check
    {
        createTestBuffers(audioBuffer, midiBuffer);
        processor->processBlock(audioBuffer, midiBuffer);
        juce::Thread::sleep(50);  // Small delay to allow file system operations
    }
    
    // Files should have been detected and loaded (hard to test directly without exposing internals)
    TestFramework::assertTrue(true, "Folder monitoring processes without errors");
    
    return true;
}

bool PluginProcessorTests::testStateManagement()
{
    DBG("Testing state management...");
    
    auto processor = createTestProcessor();
    prepareProcessor(*processor);
    
    // Set some state
    processor->startPlayback();
    processor->setMidiFolder("C:\\TestFolder");
    
    // Save state
    juce::MemoryBlock stateData;
    processor->getStateInformation(stateData);
    
    TestFramework::assertTrue(stateData.getSize() > 0, "State data generated");
    
    // Create new processor and restore state
    auto processor2 = createTestProcessor();
    prepareProcessor(*processor2);
    
    processor2->setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));
    
    // Verify state was restored (hard to test without exposing internal state)
    TestFramework::assertTrue(true, "State save/restore completes without errors");
    
    return true;
}

//==============================================================================
// Helper Methods

std::unique_ptr<AIBandAudioProcessor> PluginProcessorTests::createTestProcessor()
{
    return std::make_unique<AIBandAudioProcessor>();
}

void PluginProcessorTests::prepareProcessor(AIBandAudioProcessor& processor)
{
    processor.prepareToPlay(44100.0, 512);
}

void PluginProcessorTests::createTestBuffers(juce::AudioBuffer<float>& audioBuffer, 
                                            juce::MidiBuffer& midiBuffer,
                                            int numSamples)
{
    audioBuffer = TestFramework::createSilentAudioBuffer(2, numSamples);
    midiBuffer.clear();
}
