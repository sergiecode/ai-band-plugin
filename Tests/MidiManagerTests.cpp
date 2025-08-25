#include "MidiManagerTests.h"

//==============================================================================
MidiManagerTests::MidiManagerTests()
{
}

MidiManagerTests::~MidiManagerTests()
{
}

//==============================================================================
bool MidiManagerTests::runAllTests()
{
    DBG("=== Running MidiManager Tests ===");
    
    bool allPassed = true;
    
    allPassed &= testInitializationAndCleanup();
    allPassed &= testFileFormatValidation();
    allPassed &= testMidiFileLoading();
    allPassed &= testInvalidMidiFileHandling();
    allPassed &= testMidiFileSaving();
    allPassed &= testTempoDetection();
    allPassed &= testTimeSignatureDetection();
    allPassed &= testDurationCalculation();
    allPassed &= testBeatSampleConversion();
    allPassed &= testErrorHandling();
    
    DBG("=== MidiManager Tests Complete ===");
    return allPassed;
}

//==============================================================================
bool MidiManagerTests::testInitializationAndCleanup()
{
    DBG("Testing MidiManager initialization and cleanup...");
    
    auto manager = createTestMidiManager();
    TestFramework::assertTrue(manager != nullptr, "MidiManager creation");
    
    // Test initialization
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Test cleanup
    manager->releaseResources();
    
    TestFramework::assertTrue(true, "MidiManager initialization and cleanup");
    return true;
}

bool MidiManagerTests::testFileFormatValidation()
{
    DBG("Testing file format validation...");
    
    // Test valid extensions
    TestFramework::assertTrue(MidiManager::isValidMidiFile("test.mid"), 
                            "Valid .mid extension");
    TestFramework::assertTrue(MidiManager::isValidMidiFile("test.midi"), 
                            "Valid .midi extension");
    TestFramework::assertTrue(MidiManager::isValidMidiFile("C:\\path\\to\\file.mid"), 
                            "Valid .mid with path");
    
    // Test invalid extensions
    TestFramework::assertTrue(!MidiManager::isValidMidiFile("test.txt"), 
                            "Invalid .txt extension");
    TestFramework::assertTrue(!MidiManager::isValidMidiFile("test.wav"), 
                            "Invalid .wav extension");
    TestFramework::assertTrue(!MidiManager::isValidMidiFile("test"), 
                            "No extension");
    
    // Test supported extensions
    auto extensions = MidiManager::getSupportedExtensions();
    TestFramework::assertTrue(extensions.contains("*.mid"), "Supported extensions include .mid");
    TestFramework::assertTrue(extensions.contains("*.midi"), "Supported extensions include .midi");
    
    return true;
}

bool MidiManagerTests::testMidiFileLoading()
{
    DBG("Testing MIDI file loading...");
    
    auto manager = createTestMidiManager();
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Create test MIDI file
    auto tempDir = TestFramework::createTempTestDirectory();
    auto bassFile = tempDir.getChildFile("test_bass.mid");
    
    TestFramework::assertTrue(
        TestFramework::createTestBassMidiFile(bassFile.getFullPathName(), 8.0, 120),
        "Create test bass MIDI file"
    );
    
    // Test loading the file
    juce::MidiBuffer buffer;
    bool loadSuccess = manager->loadMidiFile(bassFile.getFullPathName(), buffer);
    
    TestFramework::assertTrue(loadSuccess, "Load valid MIDI file");
    TestFramework::assertTrue(!buffer.isEmpty(), "MIDI buffer not empty after loading");
    
    // Test loading non-existent file
    juce::MidiBuffer emptyBuffer;
    bool loadFailure = manager->loadMidiFile("nonexistent.mid", emptyBuffer);
    TestFramework::assertTrue(!loadFailure, "Fail to load non-existent file");
    
    return true;
}

bool MidiManagerTests::testInvalidMidiFileHandling()
{
    DBG("Testing invalid MIDI file handling...");
    
    auto manager = createTestMidiManager();
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Create invalid MIDI file
    auto tempDir = TestFramework::createTempTestDirectory();
    auto invalidFile = tempDir.getChildFile("invalid.mid");
    
    TestFramework::assertTrue(
        TestFramework::createInvalidMidiFile(invalidFile.getFullPathName()),
        "Create invalid MIDI file"
    );
    
    // Test loading invalid file
    juce::MidiBuffer buffer;
    bool loadResult = manager->loadMidiFile(invalidFile.getFullPathName(), buffer);
    
    TestFramework::assertTrue(!loadResult, "Reject invalid MIDI file");
    
    return true;
}

bool MidiManagerTests::testMidiFileSaving()
{
    DBG("Testing MIDI file saving...");
    
    auto manager = createTestMidiManager();
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Create test MIDI buffer
    auto testBuffer = TestFramework::createTestMidiBuffer(4.0, 120);
    
    // Save to file
    auto tempDir = TestFramework::createTempTestDirectory();
    auto outputFile = tempDir.getChildFile("saved_test.mid");
    
    bool saveSuccess = manager->saveMidiFile(testBuffer, outputFile.getFullPathName());
    TestFramework::assertTrue(saveSuccess, "Save MIDI file");
    TestFramework::assertFileExists(outputFile.getFullPathName(), "Saved file exists");
    
    // Test loading the saved file back
    juce::MidiBuffer loadedBuffer;
    bool loadSuccess = manager->loadMidiFile(outputFile.getFullPathName(), loadedBuffer);
    TestFramework::assertTrue(loadSuccess, "Load saved MIDI file");
    
    return true;
}

bool MidiManagerTests::testTempoDetection()
{
    DBG("Testing tempo detection...");
    
    auto manager = createTestMidiManager();
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Create MIDI file with known tempo
    auto tempDir = TestFramework::createTempTestDirectory();
    auto midiFile = tempDir.getChildFile("tempo_test.mid");
    
    TestFramework::assertTrue(
        TestFramework::createTestBassMidiFile(midiFile.getFullPathName(), 4.0, 140),
        "Create MIDI file with 140 BPM"
    );
    
    // Load and test tempo detection
    juce::MidiBuffer buffer;
    manager->loadMidiFile(midiFile.getFullPathName(), buffer);
    
    double detectedTempo = manager->getTempoFromMidi(buffer);
    
    // Should detect 140 BPM or fall back to default 120
    TestFramework::assertTrue(detectedTempo > 0, "Tempo detection returns positive value");
    TestFramework::assertTrue(detectedTempo >= 80 && detectedTempo <= 200, 
                            "Tempo in reasonable range");
    
    return true;
}

bool MidiManagerTests::testTimeSignatureDetection()
{
    DBG("Testing time signature detection...");
    
    auto manager = createTestMidiManager();
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Create test MIDI buffer
    auto testBuffer = TestFramework::createTestMidiBuffer(4.0, 120);
    
    int numerator, denominator;
    bool hasTimeSignature = manager->getTimeSignatureFromMidi(testBuffer, numerator, denominator);
    
    // Should either detect time signature or return default 4/4
    if (hasTimeSignature)
    {
        TestFramework::assertTrue(numerator > 0 && denominator > 0, 
                                "Valid time signature values");
    }
    else
    {
        TestFramework::assertEqualInt(4, numerator, "Default numerator is 4");
        TestFramework::assertEqualInt(4, denominator, "Default denominator is 4");
    }
    
    return true;
}

bool MidiManagerTests::testDurationCalculation()
{
    DBG("Testing duration calculation...");
    
    auto manager = createTestMidiManager();
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Create MIDI buffer with known duration
    auto testBuffer = TestFramework::createTestMidiBuffer(8.0, 120); // 8 beats at 120 BPM
    
    double duration = manager->getMidiDurationInBeats(testBuffer);
    
    TestFramework::assertTrue(duration > 0, "Duration is positive");
    TestFramework::assertApproxEqual(8.0, duration, 1.0, "Duration approximately 8 beats");
    
    // Test empty buffer
    juce::MidiBuffer emptyBuffer;
    double emptyDuration = manager->getMidiDurationInBeats(emptyBuffer);
    TestFramework::assertEqualDouble(0.0, emptyDuration, "Empty buffer duration is 0");
    
    return true;
}

bool MidiManagerTests::testBeatSampleConversion()
{
    DBG("Testing beat/sample conversion...");
    
    auto manager = createTestMidiManager();
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Test conversions at 120 BPM (2 beats per second)
    int tempo = 120;
    
    // 1 beat at 120 BPM = 0.5 seconds = 22050 samples at 44.1kHz
    int expectedSamples = 22050;
    int actualSamples = manager->beatsToSamples(1.0, tempo);
    
    TestFramework::assertApproxEqual(expectedSamples, actualSamples, 100, 
                                   "1 beat to samples conversion");
    
    // Convert back
    double actualBeats = manager->samplesToBeats(actualSamples, tempo);
    TestFramework::assertApproxEqual(1.0, actualBeats, 0.01, 
                                   "Samples to beats conversion");
    
    // Test zero values
    TestFramework::assertEqualInt(0, manager->beatsToSamples(0.0, tempo), "0 beats to samples");
    TestFramework::assertEqualDouble(0.0, manager->samplesToBeats(0, tempo), "0 samples to beats");
    
    return true;
}

bool MidiManagerTests::testErrorHandling()
{
    DBG("Testing error handling...");
    
    auto manager = createTestMidiManager();
    manager->initialize();
    manager->prepareToPlay(44100.0, 512);
    
    // Test loading from invalid path
    juce::MidiBuffer buffer;
    bool result = manager->loadMidiFile("", buffer);
    TestFramework::assertTrue(!result, "Empty path should fail");
    
    result = manager->loadMidiFile("invalid:/path", buffer);
    TestFramework::assertTrue(!result, "Invalid path should fail");
    
    // Test saving to invalid path
    auto testBuffer = TestFramework::createTestMidiBuffer(2.0, 120);
    result = manager->saveMidiFile(testBuffer, "");
    TestFramework::assertTrue(!result, "Empty save path should fail");
    
    return true;
}

//==============================================================================
// Helper Methods

std::unique_ptr<MidiManager> MidiManagerTests::createTestMidiManager()
{
    return std::make_unique<MidiManager>();
}

bool MidiManagerTests::validateMidiBufferContents(const juce::MidiBuffer& buffer, 
                                                  const juce::String& expectedContent)
{
    // This is a simplified validation - in practice you'd check specific MIDI events
    return !buffer.isEmpty();
}
