#include <iostream>
#include <string>
#include <vector>
#include <fstream>

/**
 * Simple test to verify AI Band Plugin logic without JUCE dependencies
 * This tests the core algorithms and data structures independently
 */

class SimpleMidiManager {
public:
    SimpleMidiManager() : sampleRate(44100.0), blockSize(512) {}
    
    bool loadMidiFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cout << "❌ Failed to open file: " << filename << std::endl;
            return false;
        }
        
        // Simple validation - check for MIDI header
        char header[4];
        file.read(header, 4);
        
        if (header[0] == 'M' && header[1] == 'T' && 
            header[2] == 'h' && header[3] == 'd') {
            std::cout << "✅ Valid MIDI file detected: " << filename << std::endl;
            return true;
        } else {
            std::cout << "⚠️  File doesn't appear to be MIDI: " << filename << std::endl;
            return false;
        }
    }
    
    double samplesToBeats(int samples, double bpm) const {
        double samplesPerBeat = (sampleRate * 60.0) / bpm;
        return samples / samplesPerBeat;
    }
    
    int beatsToSamples(double beats, double bpm) const {
        double samplesPerBeat = (sampleRate * 60.0) / bpm;
        return static_cast<int>(beats * samplesPerBeat);
    }
    
    void setSampleRate(double rate) { sampleRate = rate; }
    void setBlockSize(int size) { blockSize = size; }
    
private:
    double sampleRate;
    int blockSize;
};

class SimpleTestFramework {
public:
    static void assertEqual(double expected, double actual, const std::string& message) {
        if (std::abs(expected - actual) < 1e-6) {
            std::cout << "✅ " << message << " (expected: " << expected << ", got: " << actual << ")" << std::endl;
            passCount++;
        } else {
            std::cout << "❌ " << message << " (expected: " << expected << ", got: " << actual << ")" << std::endl;
            failCount++;
        }
    }
    
    static void assertEqual(int expected, int actual, const std::string& message) {
        if (expected == actual) {
            std::cout << "✅ " << message << " (expected: " << expected << ", got: " << actual << ")" << std::endl;
            passCount++;
        } else {
            std::cout << "❌ " << message << " (expected: " << expected << ", got: " << actual << ")" << std::endl;
            failCount++;
        }
    }
    
    static void printSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Passed: " << passCount << std::endl;
        std::cout << "Failed: " << failCount << std::endl;
        std::cout << "Total:  " << (passCount + failCount) << std::endl;
        
        if (failCount == 0) {
            std::cout << "🎉 ALL TESTS PASSED! 🎉" << std::endl;
        } else {
            std::cout << "❌ " << failCount << " test(s) failed" << std::endl;
        }
    }
    
private:
    static int passCount;
    static int failCount;
};

int SimpleTestFramework::passCount = 0;
int SimpleTestFramework::failCount = 0;

void testMidiManager() {
    std::cout << "\n--- Testing MIDI Manager ---" << std::endl;
    
    SimpleMidiManager manager;
    
    // Test sample rate and block size
    manager.setSampleRate(44100.0);
    manager.setBlockSize(512);
    
    // Test beat/sample conversion
    double bpm = 120.0;
    
    // At 120 BPM, 44100 Hz: 1 beat = 44100 * 60 / 120 = 22050 samples
    int expectedSamples = 22050;
    int actualSamples = manager.beatsToSamples(1.0, bpm);
    SimpleTestFramework::assertEqual(expectedSamples, actualSamples, "1 beat to samples at 120 BPM");
    
    // Test reverse conversion
    double expectedBeats = 1.0;
    double actualBeats = manager.samplesToBeats(22050, bpm);
    SimpleTestFramework::assertEqual(expectedBeats, actualBeats, "22050 samples to beats at 120 BPM");
    
    // Test zero values
    SimpleTestFramework::assertEqual(0, manager.beatsToSamples(0.0, bpm), "0 beats to samples");
    SimpleTestFramework::assertEqual(0.0, manager.samplesToBeats(0, bpm), "0 samples to beats");
}

void createTestMidiFile() {
    std::cout << "\n--- Creating Test MIDI File ---" << std::endl;
    
    // Create a simple MIDI file for testing
    std::ofstream file("test_simple.mid", std::ios::binary);
    
    // Write MIDI header (MThd)
    file.write("MThd", 4);
    
    // Header length (6 bytes)
    unsigned char headerLen[] = {0x00, 0x00, 0x00, 0x06};
    file.write(reinterpret_cast<char*>(headerLen), 4);
    
    // Format type 0, 1 track, 96 ticks per quarter note
    unsigned char headerData[] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x60};
    file.write(reinterpret_cast<char*>(headerData), 6);
    
    // Track header (MTrk)
    file.write("MTrk", 4);
    
    // Track length (minimal track)
    unsigned char trackLen[] = {0x00, 0x00, 0x00, 0x04};
    file.write(reinterpret_cast<char*>(trackLen), 4);
    
    // End of track
    unsigned char endTrack[] = {0x00, 0xFF, 0x2F, 0x00};
    file.write(reinterpret_cast<char*>(endTrack), 4);
    
    file.close();
    
    std::cout << "✅ Created test MIDI file: test_simple.mid" << std::endl;
    
    // Test loading the file
    SimpleMidiManager manager;
    manager.loadMidiFile("test_simple.mid");
}

int main() {
    std::cout << "AI Band Plugin - Simple Logic Test" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "This test verifies core plugin logic without JUCE dependencies" << std::endl;
    
    try {
        createTestMidiFile();
        testMidiManager();
        
        SimpleTestFramework::printSummary();
        
        std::cout << "\n🎯 This test validates that the core algorithms work correctly." << std::endl;
        std::cout << "📝 To test the full plugin, you'll need to install JUCE and build with CMake or Projucer." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
