#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>

// Simplified JUCE-like types for testing without JUCE
namespace SimpleJuce {
    
class String {
public:
    String() = default;
    String(const char* str) : data(str) {}
    String(const std::string& str) : data(str) {}
    
    const char* toRawUTF8() const { return data.c_str(); }
    std::string toStdString() const { return data; }
    bool isEmpty() const { return data.empty(); }
    
    String& operator=(const char* str) { data = str; return *this; }
    String& operator=(const std::string& str) { data = str; return *this; }
    bool operator==(const String& other) const { return data == other.data; }
    
private:
    std::string data;
};

class File {
public:
    File(const String& path) : filepath(path.toStdString()) {}
    
    bool existsAsFile() const {
        std::ifstream file(filepath);
        return file.good();
    }
    
    String getFullPathName() const { return String(filepath); }
    
private:
    std::string filepath;
};

template<typename T>
class AudioBuffer {
public:
    AudioBuffer(int channels, int samples) : numChannels(channels), numSamples(samples) {
        data.resize(channels * samples, T(0));
    }
    
    int getNumChannels() const { return numChannels; }
    int getNumSamples() const { return numSamples; }
    
private:
    std::vector<T> data;
    int numChannels, numSamples;
};

class MidiBuffer {
public:
    void clear() { /* Implementation */ }
    bool isEmpty() const { return true; }
    void addEvent(const void* data, int size, int time) { /* Implementation */ }
};

class MidiMessage {
public:
    static MidiMessage noteOn(int channel, int note, float velocity) {
        return MidiMessage();
    }
    static MidiMessage noteOff(int channel, int note, float velocity) {
        return MidiMessage();
    }
};

class MidiFile {
public:
    bool loadFrom(const void* data, size_t size) { return true; }
    int getNumTracks() const { return 1; }
    double getTickLength() const { return 0.0; }
    void convertTimestampTicksToSeconds() {}
};

} // namespace SimpleJuce

// Define macros to use our simple types
#define juce SimpleJuce
#define DBG(x) std::cout << x << std::endl
