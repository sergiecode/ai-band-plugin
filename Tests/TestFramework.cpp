#include "TestFramework.h"

//==============================================================================
// Static member initialization
int TestFramework::testsRun = 0;
int TestFramework::testsPassed = 0;
int TestFramework::testsFailed = 0;
juce::Array<juce::File> TestFramework::tempFilesToCleanup;

//==============================================================================
TestFramework::TestFramework()
{
}

TestFramework::~TestFramework()
{
}

//==============================================================================
// Test Environment Setup

bool TestFramework::initialize()
{
    resetCounters();
    tempFilesToCleanup.clear();
    
    DBG("TestFramework initialized");
    return true;
}

void TestFramework::cleanup()
{
    cleanupTempFiles();
    DBG("TestFramework cleanup complete");
}

juce::File TestFramework::createTempTestDirectory()
{
    auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                   .getChildFile("AIBandPluginTests")
                   .getChildFile(juce::Uuid().toString());
    
    if (tempDir.createDirectory())
    {
        tempFilesToCleanup.add(tempDir);
        return tempDir;
    }
    
    return juce::File();
}

void TestFramework::cleanupTempFiles()
{
    for (auto& file : tempFilesToCleanup)
    {
        if (file.exists())
        {
            if (file.isDirectory())
                file.deleteRecursively();
            else
                file.deleteFile();
        }
    }
    tempFilesToCleanup.clear();
}

//==============================================================================
// MIDI Test Utilities

bool TestFramework::createTestBassMidiFile(const juce::String& filePath, double durationInBeats, int tempo)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(480);
    
    juce::MidiMessageSequence track;
    
    // Add tempo meta event
    track.addEvent(juce::MidiMessage::tempoMetaEvent(60000000 / tempo), 0.0);
    
    // Add time signature (4/4)
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0.0);
    
    // Create simple bass pattern (C2, F2, G2, C2)
    juce::Array<int> bassNotes = {36, 41, 43, 36}; // C2, F2, G2, C2
    double beatDuration = 1.0; // One beat per note
    
    for (int beat = 0; beat < durationInBeats; ++beat)
    {
        int noteIndex = beat % bassNotes.size();
        int note = bassNotes[noteIndex];
        double startTime = beat * beatDuration;
        double endTime = startTime + beatDuration * 0.9; // Slight gap between notes
        
        // Note on
        track.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8)80), startTime);
        // Note off
        track.addEvent(juce::MidiMessage::noteOff(1, note), endTime);
    }
    
    midiFile.addTrack(track);
    
    juce::File file(filePath);
    juce::FileOutputStream stream(file);
    
    if (stream.failedToOpen())
        return false;
    
    midiFile.writeTo(stream);
    tempFilesToCleanup.add(file);
    
    return !stream.getStatus().failed();
}

bool TestFramework::createTestDrumMidiFile(const juce::String& filePath, double durationInBeats, int tempo)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(480);
    
    juce::MidiMessageSequence track;
    
    // Add tempo meta event
    track.addEvent(juce::MidiMessage::tempoMetaEvent(60000000 / tempo), 0.0);
    
    // Add time signature (4/4)
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0.0);
    
    // Drum notes (General MIDI)
    int kick = 36;  // Bass Drum 1
    int snare = 38; // Acoustic Snare
    int hihat = 42; // Closed Hi-Hat
    
    // Simple 4/4 pattern
    for (int beat = 0; beat < durationInBeats; ++beat)
    {
        double beatTime = beat * 1.0;
        
        // Kick on beats 1 and 3
        if (beat % 4 == 0 || beat % 4 == 2)
        {
            track.addEvent(juce::MidiMessage::noteOn(10, kick, (juce::uint8)100), beatTime);
            track.addEvent(juce::MidiMessage::noteOff(10, kick), beatTime + 0.1);
        }
        
        // Snare on beats 2 and 4
        if (beat % 4 == 1 || beat % 4 == 3)
        {
            track.addEvent(juce::MidiMessage::noteOn(10, snare, (juce::uint8)90), beatTime);
            track.addEvent(juce::MidiMessage::noteOff(10, snare), beatTime + 0.1);
        }
        
        // Hi-hat on every beat
        track.addEvent(juce::MidiMessage::noteOn(10, hihat, (juce::uint8)60), beatTime);
        track.addEvent(juce::MidiMessage::noteOff(10, hihat), beatTime + 0.4);
    }
    
    midiFile.addTrack(track);
    
    juce::File file(filePath);
    juce::FileOutputStream stream(file);
    
    if (stream.failedToOpen())
        return false;
    
    midiFile.writeTo(stream);
    tempFilesToCleanup.add(file);
    
    return !stream.getStatus().failed();
}

bool TestFramework::createInvalidMidiFile(const juce::String& filePath)
{
    juce::File file(filePath);
    juce::FileOutputStream stream(file);
    
    if (stream.failedToOpen())
        return false;
    
    // Write invalid MIDI data
    stream.writeString("This is not a valid MIDI file");
    tempFilesToCleanup.add(file);
    
    return !stream.getStatus().failed();
}

bool TestFramework::validateMidiBuffer(const juce::MidiBuffer& buffer, int expectedEventCount, double expectedDuration)
{
    int eventCount = 0;
    int lastSamplePosition = 0;
    
    for (auto it = buffer.cbegin(); it != buffer.cend(); ++it)
    {
        eventCount++;
        auto metadata = (*it);
        lastSamplePosition = juce::jmax(lastSamplePosition, metadata.samplePosition);
    }
    
    bool valid = true;
    
    if (expectedEventCount >= 0 && eventCount != expectedEventCount)
    {
        DBG("MIDI validation failed: Expected " << expectedEventCount << " events, got " << eventCount);
        valid = false;
    }
    
    if (expectedDuration >= 0.0)
    {
        double actualDuration = lastSamplePosition / 44100.0; // Assume 44.1kHz
        if (juce::approximatelyEqual(actualDuration, expectedDuration, 0.1) == false)
        {
            DBG("MIDI validation failed: Expected duration " << expectedDuration << "s, got " << actualDuration << "s");
            valid = false;
        }
    }
    
    return valid;
}

//==============================================================================
// Audio Test Utilities

juce::AudioBuffer<float> TestFramework::createSilentAudioBuffer(int numChannels, int numSamples)
{
    juce::AudioBuffer<float> buffer(numChannels, numSamples);
    buffer.clear();
    return buffer;
}

juce::MidiBuffer TestFramework::createTestMidiBuffer(double durationInBeats, int tempo)
{
    juce::MidiBuffer buffer;
    
    double beatsPerSecond = tempo / 60.0;
    double durationInSeconds = durationInBeats / beatsPerSecond;
    int durationInSamples = static_cast<int>(durationInSeconds * 44100.0); // 44.1kHz
    
    // Add some test MIDI events
    for (int beat = 0; beat < static_cast<int>(durationInBeats); ++beat)
    {
        int samplePosition = static_cast<int>((beat / beatsPerSecond) * 44100.0);
        
        // Add note on/off events
        buffer.addEvent(juce::MidiMessage::noteOn(1, 60 + beat % 12, (juce::uint8)80), samplePosition);
        buffer.addEvent(juce::MidiMessage::noteOff(1, 60 + beat % 12), samplePosition + 2205); // 50ms later
    }
    
    return buffer;
}

bool TestFramework::validateAudioOutput(const juce::AudioBuffer<float>& buffer, bool expectSilence)
{
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();
    
    if (numChannels <= 0 || numSamples <= 0)
        return false;
    
    bool hasAudio = false;
    bool hasClipping = false;
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float value = channelData[sample];
            
            if (std::abs(value) > 0.001f)
                hasAudio = true;
            
            if (std::abs(value) > 1.0f)
                hasClipping = true;
        }
    }
    
    if (expectSilence && hasAudio)
    {
        DBG("Audio validation failed: Expected silence but found audio signal");
        return false;
    }
    
    if (hasClipping)
    {
        DBG("Audio validation failed: Clipping detected");
        return false;
    }
    
    return true;
}

//==============================================================================
// Assertion Helpers

void TestFramework::assertTrue(bool condition, const juce::String& message)
{
    incrementTestCount();
    
    if (condition)
    {
        incrementPassCount();
        DBG("PASS: " << message);
    }
    else
    {
        incrementFailCount();
        DBG("FAIL: " << message);
    }
}

void TestFramework::assertEqualInt(int expected, int actual, const juce::String& message)
{
    incrementTestCount();
    
    if (expected == actual)
    {
        incrementPassCount();
        DBG("PASS: " << message);
    }
    else
    {
        incrementFailCount();
        DBG("FAIL: " << message << " - Expected: " << expected << ", Actual: " << actual);
    }
}

void TestFramework::assertEqualDouble(double expected, double actual, const juce::String& message)
{
    incrementTestCount();
    
    if (expected == actual)
    {
        incrementPassCount();
        DBG("PASS: " << message);
    }
    else
    {
        incrementFailCount();
        DBG("FAIL: " << message << " - Expected: " << expected << ", Actual: " << actual);
    }
}

void TestFramework::assertEqualString(const juce::String& expected, const juce::String& actual, const juce::String& message)
{
    incrementTestCount();
    
    if (expected == actual)
    {
        incrementPassCount();
        DBG("PASS: " << message);
    }
    else
    {
        incrementFailCount();
        DBG("FAIL: " << message << " - Expected: " << expected << ", Actual: " << actual);
    }
}

void TestFramework::assertApproxEqual(double expected, double actual, double tolerance, const juce::String& message)
{
    incrementTestCount();
    
    if (std::abs(expected - actual) <= tolerance)
    {
        incrementPassCount();
        DBG("PASS: " << message);
    }
    else
    {
        incrementFailCount();
        DBG("FAIL: " << message << " - Expected: " << expected << ", Actual: " << actual << ", Tolerance: " << tolerance);
    }
}

void TestFramework::assertFileExists(const juce::String& filePath, const juce::String& message)
{
    incrementTestCount();
    
    juce::File file(filePath);
    if (file.exists())
    {
        incrementPassCount();
        DBG("PASS: " << message);
    }
    else
    {
        incrementFailCount();
        DBG("FAIL: " << message << " - File does not exist: " << filePath);
    }
}

//==============================================================================
// Test Result Tracking

void TestFramework::resetCounters()
{
    testsRun = 0;
    testsPassed = 0;
    testsFailed = 0;
}

void TestFramework::printTestSummary()
{
    DBG("=== Test Summary ===");
    DBG("Tests Run: " << testsRun);
    DBG("Tests Passed: " << testsPassed);
    DBG("Tests Failed: " << testsFailed);
    
    if (testsFailed == 0)
        DBG("All tests PASSED!");
    else
        DBG("Some tests FAILED!");
}

//==============================================================================
// Private Methods

void TestFramework::incrementTestCount()
{
    testsRun++;
}

void TestFramework::incrementPassCount()
{
    testsPassed++;
}

void TestFramework::incrementFailCount()
{
    testsFailed++;
}
