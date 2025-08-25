#include "MidiManager.h"

//==============================================================================
MidiManager::MidiManager()
    : currentSampleRate(44100.0),
      currentBlockSize(512)
{
}

MidiManager::~MidiManager()
{
}

//==============================================================================
void MidiManager::initialize()
{
    // Initialize any required components
}

void MidiManager::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
}

void MidiManager::releaseResources()
{
    // Clean up any resources
}

//==============================================================================
bool MidiManager::loadMidiFile(const juce::String& filePath, juce::MidiBuffer& buffer)
{
    juce::File midiFile(filePath);
    
    if (!midiFile.exists())
    {
        DBG("MIDI file does not exist: " << filePath);
        return false;
    }
    
    if (!isValidMidiFile(filePath))
    {
        DBG("Invalid MIDI file: " << filePath);
        return false;
    }
    
    juce::FileInputStream stream(midiFile);
    if (stream.failedToOpen())
    {
        DBG("Failed to open MIDI file: " << filePath);
        return false;
    }
    
    juce::MidiFile loadedMidiFile;
    if (!loadedMidiFile.readFrom(stream))
    {
        DBG("Failed to read MIDI file: " << filePath);
        return false;
    }
    
    // Convert MidiFile to MidiBuffer
    buffer.clear();
    convertMidiFileToBuffer(loadedMidiFile, buffer);
    
    DBG("Successfully loaded MIDI file: " << filePath << 
        " Duration: " << getMidiDurationInBeats(buffer) << " beats");
    
    return true;
}

bool MidiManager::loadMidiFromMemory(const void* data, size_t size, juce::MidiBuffer& buffer)
{
    if (data == nullptr || size == 0)
        return false;
    
    juce::MemoryInputStream stream(data, size, false);
    juce::MidiFile midiFile;
    
    if (!midiFile.readFrom(stream))
    {
        DBG("Failed to read MIDI data from memory");
        return false;
    }
    
    buffer.clear();
    convertMidiFileToBuffer(midiFile, buffer);
    
    return true;
}

bool MidiManager::saveMidiFile(const juce::MidiBuffer& buffer, const juce::String& filePath)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(480); // Standard MIDI resolution
    
    // Create a single track from the buffer
    juce::MidiMessageSequence track;
    
    for (auto it = buffer.cbegin(); it != buffer.cend(); ++it)
    {
        auto metadata = (*it);
        double timeInSeconds = metadata.samplePosition / currentSampleRate;
        track.addEvent(metadata.getMessage(), timeInSeconds);
    }
    
    midiFile.addTrack(track);
    
    // Save to file
    juce::File outputFile(filePath);
    juce::FileOutputStream stream(outputFile);
    
    if (stream.failedToOpen())
    {
        DBG("Failed to create output file: " << filePath);
        return false;
    }
    
    midiFile.writeTo(stream);
    return !stream.getStatus().failed();
}

//==============================================================================
double MidiManager::getMidiDurationInBeats(const juce::MidiBuffer& buffer) const
{
    if (buffer.isEmpty())
        return 0.0;
    
    int lastSample = 0;
    for (auto it = buffer.cbegin(); it != buffer.cend(); ++it)
    {
        auto metadata = (*it);
        lastSample = juce::jmax(lastSample, metadata.samplePosition);
    }
    
    // Convert samples to beats (assuming 120 BPM default)
    double timeInSeconds = lastSample / currentSampleRate;
    return (timeInSeconds * 120.0) / 60.0; // 120 BPM = 2 beats per second
}

double MidiManager::getTempoFromMidi(const juce::MidiBuffer& buffer) const
{
    // Look for tempo change events in the buffer
    for (auto it = buffer.cbegin(); it != buffer.cend(); ++it)
    {
        auto metadata = (*it);
        auto message = metadata.getMessage();
        
        if (message.isTempoMetaEvent())
        {
            return message.getTempoSecondsPerQuarterNote() > 0 ? 
                   60.0 / message.getTempoSecondsPerQuarterNote() : 120.0;
        }
    }
    
    return 120.0; // Default tempo if none found
}

bool MidiManager::getTimeSignatureFromMidi(const juce::MidiBuffer& buffer, int& numerator, int& denominator) const
{
    // Look for time signature events in the buffer
    for (auto it = buffer.cbegin(); it != buffer.cend(); ++it)
    {
        auto metadata = (*it);
        auto message = metadata.getMessage();
        
        if (message.isTimeSignatureMetaEvent())
        {
            message.getTimeSignatureInfo(numerator, denominator);
            return true;
        }
    }
    
    // Default to 4/4 if not found
    numerator = 4;
    denominator = 4;
    return false;
}

//==============================================================================
int MidiManager::beatsToSamples(double beats, double tempo) const
{
    double secondsPerBeat = 60.0 / tempo;
    double timeInSeconds = beats * secondsPerBeat;
    return static_cast<int>(timeInSeconds * currentSampleRate);
}

double MidiManager::samplesToBeats(int samples, double tempo) const
{
    double timeInSeconds = samples / currentSampleRate;
    double secondsPerBeat = 60.0 / tempo;
    return timeInSeconds / secondsPerBeat;
}

//==============================================================================
bool MidiManager::isValidMidiFile(const juce::String& filePath)
{
    juce::File file(filePath);
    
    if (!file.exists())
        return false;
    
    auto extension = file.getFileExtension().toLowerCase();
    return extension == ".mid" || extension == ".midi";
}

juce::StringArray MidiManager::getSupportedExtensions()
{
    return { "*.mid", "*.midi" };
}

//==============================================================================
// Private methods

void MidiManager::convertMidiFileToBuffer(const juce::MidiFile& midiFile, juce::MidiBuffer& buffer, double tempoScale)
{
    buffer.clear();
    
    int ticksPerBeat = midiFile.getTimeFormat();
    if (ticksPerBeat <= 0)
        ticksPerBeat = 480; // Default resolution
    
    // Process each track
    for (int trackIndex = 0; trackIndex < midiFile.getNumTracks(); ++trackIndex)
    {
        const auto* track = midiFile.getTrack(trackIndex);
        if (track == nullptr)
            continue;
        
        // Build tempo map for this file
        auto tempoMap = buildTempoMap(*track, ticksPerBeat);
        
        // Convert each event in the track
        for (int eventIndex = 0; eventIndex < track->getNumEvents(); ++eventIndex)
        {
            auto* event = track->getEventPointer(eventIndex);
            if (event == nullptr)
                continue;
            
            auto message = event->message;
            
            // Skip meta events (except tempo which we handle separately)
            if (message.isMetaEvent() && !message.isTempoMetaEvent())
                continue;
            
            // Convert tick time to seconds, then to samples
            double timeInSeconds = ticksToSeconds(message.getTimeStamp(), tempoMap, ticksPerBeat);
            timeInSeconds *= tempoScale; // Apply tempo scaling
            
            int samplePosition = static_cast<int>(timeInSeconds * currentSampleRate);
            
            // Add to buffer (handle note-offs for sustained notes)
            buffer.addEvent(message, samplePosition);
        }
    }
}

std::vector<MidiManager::TempoEvent> MidiManager::buildTempoMap(const juce::MidiMessageSequence& track, int ticksPerBeat)
{
    std::vector<TempoEvent> tempoMap;
    
    // Add default tempo at start
    TempoEvent defaultTempo;
    defaultTempo.tick = 0;
    defaultTempo.tempo = 120.0; // Default 120 BPM
    defaultTempo.timeInSeconds = 0.0;
    tempoMap.push_back(defaultTempo);
    
    // Scan for tempo change events
    for (int i = 0; i < track.getNumEvents(); ++i)
    {
        auto* event = track.getEventPointer(i);
        if (event == nullptr)
            continue;
        
        auto message = event->message;
        if (message.isTempoMetaEvent())
        {
            TempoEvent tempoEvent;
            tempoEvent.tick = message.getTimeStamp();
            tempoEvent.tempo = message.getTempoSecondsPerQuarterNote() > 0 ? 
                              60.0 / message.getTempoSecondsPerQuarterNote() : 120.0;
            
            // Calculate time in seconds based on previous tempo
            if (!tempoMap.empty())
            {
                auto& lastTempo = tempoMap.back();
                int tickDelta = tempoEvent.tick - lastTempo.tick;
                double beatDelta = static_cast<double>(tickDelta) / ticksPerBeat;
                double timeDelta = beatDelta * (60.0 / lastTempo.tempo);
                tempoEvent.timeInSeconds = lastTempo.timeInSeconds + timeDelta;
            }
            else
            {
                tempoEvent.timeInSeconds = 0.0;
            }
            
            tempoMap.push_back(tempoEvent);
        }
    }
    
    return tempoMap;
}

double MidiManager::ticksToSeconds(int ticks, const std::vector<TempoEvent>& tempoMap, int ticksPerBeat) const
{
    if (tempoMap.empty())
        return 0.0;
    
    // Find the tempo event that applies to this tick position
    TempoEvent currentTempo = tempoMap[0];
    
    for (const auto& tempoEvent : tempoMap)
    {
        if (tempoEvent.tick <= ticks)
            currentTempo = tempoEvent;
        else
            break;
    }
    
    // Calculate time based on the current tempo
    int tickDelta = ticks - currentTempo.tick;
    double beatDelta = static_cast<double>(tickDelta) / ticksPerBeat;
    double timeDelta = beatDelta * (60.0 / currentTempo.tempo);
    
    return currentTempo.timeInSeconds + timeDelta;
}
