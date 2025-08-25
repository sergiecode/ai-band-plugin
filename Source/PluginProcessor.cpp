#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AIBandAudioProcessor::AIBandAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
       isPlayingTracks(false),
       currentBeat(0.0),
       beatsPerSecond(2.0), // Default 120 BPM = 2 beats per second
       samplesSinceLastBeat(0),
       hostSampleRate(44100.0),
       hostBlockSize(512)
{
    // Initialize MIDI manager and network client
    midiManager.initialize();
    networkClient.initialize();
}

AIBandAudioProcessor::~AIBandAudioProcessor()
{
}

//==============================================================================
const juce::String AIBandAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AIBandAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AIBandAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AIBandAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AIBandAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AIBandAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AIBandAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AIBandAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String AIBandAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AIBandAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AIBandAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    hostSampleRate = sampleRate;
    hostBlockSize = samplesPerBlock;
    
    // Calculate beats per second based on host tempo (if available)
    // Default to 120 BPM if no tempo info available
    auto playhead = getPlayHead();
    if (playhead != nullptr)
    {
        juce::AudioPlayHead::CurrentPositionInfo positionInfo;
        if (playhead->getCurrentPosition(positionInfo))
        {
            if (positionInfo.bpm > 0)
                beatsPerSecond = positionInfo.bpm / 60.0;
        }
    }
    
    // Prepare MIDI manager
    midiManager.prepareToPlay(sampleRate, samplesPerBlock);
    
    // Reset playback state
    currentBeat = 0.0;
    samplesSinceLastBeat = 0;
}

void AIBandAudioProcessor::releaseResources()
{
    midiManager.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AIBandAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AIBandAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Update playback position and tempo
    updatePlaybackPosition(buffer.getNumSamples());
    
    // Check for new MIDI files periodically
    checkForNewMidiFiles();
    
    // Process MIDI events if we're playing
    if (isPlayingTracks)
    {
        processMidiEvents(midiMessages, buffer.getNumSamples());
    }
    
    // Pass through input audio (if any)
    for (int channel = 0; channel < juce::jmin(totalNumInputChannels, totalNumOutputChannels); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        // Audio pass-through happens automatically since we're not modifying the buffer
        juce::ignoreUnused(channelData);
    }
}

//==============================================================================
bool AIBandAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AIBandAudioProcessor::createEditor()
{
    return new AIBandAudioProcessorEditor (*this);
}

//==============================================================================
void AIBandAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::ValueTree state("AIBandPlugin");
    state.setProperty("isPlaying", isPlayingTracks, nullptr);
    state.setProperty("currentBeat", currentBeat, nullptr);
    state.setProperty("monitoredFolder", monitoredFolder, nullptr);
    
    juce::MemoryOutputStream stream(destData, false);
    state.writeToStream(stream);
}

void AIBandAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::ValueTree state = juce::ValueTree::readFromData(data, sizeInBytes);
    
    if (state.isValid() && state.hasType("AIBandPlugin"))
    {
        isPlayingTracks = state.getProperty("isPlaying", false);
        currentBeat = state.getProperty("currentBeat", 0.0);
        monitoredFolder = state.getProperty("monitoredFolder", "");
    }
}

//==============================================================================
// AI Band specific functionality

bool AIBandAudioProcessor::loadMidiFiles(const juce::String& bassFilePath, const juce::String& drumFilePath)
{
    bool success = true;
    
    // Load bass MIDI file
    if (bassFilePath.isNotEmpty())
    {
        success &= midiManager.loadMidiFile(bassFilePath, bassMidiData);
    }
    
    // Load drum MIDI file  
    if (drumFilePath.isNotEmpty())
    {
        success &= midiManager.loadMidiFile(drumFilePath, drumMidiData);
    }
    
    if (success)
    {
        // Reset playback position when new files are loaded
        resetPlayback();
    }
    
    return success;
}

void AIBandAudioProcessor::startPlayback()
{
    isPlayingTracks = true;
    resetPlayback();
}

void AIBandAudioProcessor::stopPlayback()
{
    isPlayingTracks = false;
    
    // Send all-notes-off MIDI messages
    juce::MidiBuffer allNotesOff;
    for (int channel = 1; channel <= 16; ++channel)
    {
        allNotesOff.addEvent(juce::MidiMessage::allNotesOff(channel), 0);
    }
    
    // This will be sent in the next processBlock call
    currentMidiBuffer.swapWith(allNotesOff);
}

void AIBandAudioProcessor::setMidiFolder(const juce::String& folderPath)
{
    monitoredFolder = folderPath;
    lastFolderCheck = juce::Time::getCurrentTime();
}

void AIBandAudioProcessor::resetPlayback()
{
    currentBeat = 0.0;
    samplesSinceLastBeat = 0;
}

//==============================================================================
// Internal methods

void AIBandAudioProcessor::processMidiEvents(juce::MidiBuffer& midiMessages, int numSamples)
{
    // Calculate the beat range for this audio block
    double startBeat = currentBeat;
    double endBeat = currentBeat + (numSamples / hostSampleRate) * beatsPerSecond;
    
    // Load MIDI events for this time range
    currentMidiBuffer.clear();
    loadMidiFromBuffer(bassMidiData, currentMidiBuffer, startBeat, endBeat);
    loadMidiFromBuffer(drumMidiData, currentMidiBuffer, startBeat, endBeat);
    
    // Add the generated MIDI events to the output
    midiMessages.addEvents(currentMidiBuffer, 0, numSamples, 0);
}

void AIBandAudioProcessor::updatePlaybackPosition(int numSamples)
{
    // Update beat position based on host transport or internal clock
    auto playhead = getPlayHead();
    if (playhead != nullptr)
    {
        juce::AudioPlayHead::CurrentPositionInfo positionInfo;
        if (playhead->getCurrentPosition(positionInfo))
        {
            // Use host transport if available
            if (positionInfo.isPlaying && positionInfo.ppqPosition >= 0)
            {
                currentBeat = positionInfo.ppqPosition;
                if (positionInfo.bpm > 0)
                    beatsPerSecond = positionInfo.bpm / 60.0;
                return;
            }
        }
    }
    
    // Fall back to internal clock
    samplesSinceLastBeat += numSamples;
    double samplesPerBeat = hostSampleRate / beatsPerSecond;
    
    while (samplesSinceLastBeat >= samplesPerBeat)
    {
        currentBeat += 1.0;
        samplesSinceLastBeat -= static_cast<int>(samplesPerBeat);
    }
}

void AIBandAudioProcessor::checkForNewMidiFiles()
{
    // Check for new files every 500ms to avoid excessive file system calls
    auto now = juce::Time::getCurrentTime();
    if (now.toMilliseconds() - lastFolderCheck.toMilliseconds() < 500)
        return;
        
    lastFolderCheck = now;
    
    if (monitoredFolder.isEmpty())
        return;
        
    juce::File folder(monitoredFolder);
    if (!folder.exists() || !folder.isDirectory())
        return;
        
    // Look for bass and drum MIDI files
    auto files = folder.findChildFiles(juce::File::findFiles, false, "*.mid");
    
    juce::String bassFile, drumFile;
    for (auto& file : files)
    {
        auto filename = file.getFileName().toLowerCase();
        if (filename.contains("bass"))
            bassFile = file.getFullPathName();
        else if (filename.contains("drum"))
            drumFile = file.getFullPathName();
    }
    
    // Load new files if found
    if (bassFile.isNotEmpty() || drumFile.isNotEmpty())
    {
        loadMidiFiles(bassFile, drumFile);
    }
}

void AIBandAudioProcessor::loadMidiFromBuffer(const juce::MidiBuffer& source, juce::MidiBuffer& destination, double startBeat, double endBeat)
{
    // Convert beat positions to sample positions for this block
    double samplesPerBeat = hostSampleRate / beatsPerSecond;
    int startSample = static_cast<int>((startBeat) * samplesPerBeat);
    int endSample = static_cast<int>((endBeat) * samplesPerBeat);
    
    // Extract MIDI events within the time range
    for (auto it = source.cbegin(); it != source.cend(); ++it)
    {
        auto metadata = (*it);
        int eventSample = metadata.samplePosition;
        
        // Check if event falls within our time range
        if (eventSample >= startSample && eventSample < endSample)
        {
            // Calculate relative position within this audio block
            int relativeSample = eventSample - startSample;
            destination.addEvent(metadata.getMessage(), relativeSample);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AIBandAudioProcessor();
}
