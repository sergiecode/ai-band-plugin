#pragma once

#include <JuceHeader.h>
#include "MidiManager.h"
#include "NetworkClient.h"

//==============================================================================
/**
    AI Band Plugin Processor
    
    Created by Sergie Code - Software Engineer & Programming Educator
    Part of the AI Band Ecosystem for musicians
    YouTube: https://www.youtube.com/@SergieCode
    GitHub: https://github.com/sergiecode
    
    This class handles the core audio processing and MIDI functionality.
    It integrates with the ai-band-backend to play AI-generated bass and drum tracks.
*/
class AIBandAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    AIBandAudioProcessor();
    ~AIBandAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // AI Band specific functionality
    
    /** Load MIDI files from ai-band-backend output */
    bool loadMidiFiles(const juce::String& bassFilePath, const juce::String& drumFilePath);
    
    /** Start playing the loaded MIDI tracks */
    void startPlayback();
    
    /** Stop playing the MIDI tracks */
    void stopPlayback();
    
    /** Check if MIDI tracks are currently playing */
    bool isPlaying() const { return isPlayingTracks; }
    
    /** Set the folder to monitor for new MIDI files */
    void setMidiFolder(const juce::String& folderPath);
    
    /** Get current playback position in beats */
    double getCurrentBeat() const { return currentBeat; }
    
    /** Reset playback position to beginning */
    void resetPlayback();

private:
    //==============================================================================
    // Core components
    MidiManager midiManager;
    NetworkClient networkClient;
    
    // Playback state
    bool isPlayingTracks;
    double currentBeat;
    double beatsPerSecond;
    int samplesSinceLastBeat;
    
    // MIDI data
    juce::MidiBuffer currentMidiBuffer;
    juce::MidiBuffer bassMidiData;
    juce::MidiBuffer drumMidiData;
    
    // Timing
    double hostSampleRate;
    int hostBlockSize;
    
    // File monitoring
    juce::String monitoredFolder;
    juce::Time lastFolderCheck;
    
    //==============================================================================
    // Internal methods
    void processMidiEvents(juce::MidiBuffer& midiMessages, int numSamples);
    void updatePlaybackPosition(int numSamples);
    void checkForNewMidiFiles();
    void loadMidiFromBuffer(const juce::MidiBuffer& source, juce::MidiBuffer& destination, double startBeat, double endBeat);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AIBandAudioProcessor)
};
