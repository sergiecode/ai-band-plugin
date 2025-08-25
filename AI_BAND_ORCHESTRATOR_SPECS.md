# AI Band Plugin - Reference Architecture for Orchestrator Integration

## Overview

This document provides detailed specifications for creating the **ai-band-orchestrator** - a lightweight Python server that coordinates between the AI backend and the DAW plugin. Use this as a reference to understand the plugin's architecture, communication protocols, and integration requirements.

## Project Context

The **ai-band-plugin** is a C++ JUCE audio plugin that:
- Loads AI-generated MIDI files from the ai-band-backend
- Provides real-time MIDI playback synchronized with DAW transport
- Includes placeholders for orchestrator communication via NetworkClient
- Monitors folders for automatic MIDI file detection

## Plugin Architecture Reference

### Core Components

#### 1. PluginProcessor (Audio Processing Core)
```cpp
// Key methods the orchestrator needs to interact with:
class AIBandAudioProcessor : public juce::AudioProcessor
{
    // Transport control
    void startPlayback();
    void stopPlayback();
    void resetPlayback();
    
    // File management
    void loadMidiFiles(const juce::Array<juce::File>& files);
    void setMidiFolder(const juce::String& folderPath);
    
    // Real-time state
    double getCurrentBeat() const;
    bool isPlaying() const;
    
    // Network communication placeholder
    void setNetworkClient(std::unique_ptr<NetworkClient> client);
};
```

**Orchestrator Integration Points:**
- The plugin expects MIDI files in a monitored folder
- Real-time transport synchronization via network commands
- Beat position tracking for synchronized playback

#### 2. MidiManager (File Handling)
```cpp
class MidiManager
{
    // File operations the orchestrator should trigger
    bool loadMidiFile(const juce::String& filename);
    bool saveMidiFile(const juce::String& filename, const juce::MidiBuffer& buffer);
    
    // Format conversion for network transmission
    juce::MidiBuffer convertMidiFileToBuffer(const juce::MidiFile& file);
    
    // Timing utilities for orchestrator synchronization
    double samplesToBeats(int samples, double bpm) const;
    int beatsToSamples(double beats, double bpm) const;
};
```

**File Format Requirements:**
- Standard MIDI files (.mid)
- Generated files should include tempo and time signature metadata
- File naming convention: `{timestamp}_{track_type}_{key}.mid`

#### 3. NetworkClient (Orchestrator Communication)
```cpp
class NetworkClient
{
    // Communication interface for orchestrator
    bool connect(const juce::String& serverUrl);
    void disconnect();
    
    // Commands the orchestrator should implement
    void sendTransportState(bool isPlaying, double currentBeat);
    void requestNewMIDI(const juce::String& parameters);
    void sendPluginStatus(const juce::String& status);
    
    // Event handling from orchestrator
    std::function<void(const juce::String&)> onCommandReceived;
    std::function<void(const juce::String&)> onMidiFileReceived;
};
```

## Integration Requirements for Orchestrator

### 1. Communication Protocol

The orchestrator should implement these endpoints:

#### REST API Endpoints
```python
# Plugin registration and status
POST /api/plugin/register
GET  /api/plugin/status
POST /api/plugin/heartbeat

# Transport control
POST /api/transport/start
POST /api/transport/stop
POST /api/transport/reset
GET  /api/transport/position

# MIDI generation and delivery
POST /api/midi/generate
GET  /api/midi/list
POST /api/midi/stream
DELETE /api/midi/{file_id}

# Real-time synchronization
WebSocket /ws/sync
```

#### WebSocket Messages
```json
// Transport synchronization
{
    "type": "transport_sync",
    "isPlaying": true,
    "currentBeat": 16.75,
    "tempo": 120.0,
    "timestamp": "2025-08-24T10:30:00Z"
}

// MIDI file notification
{
    "type": "midi_ready",
    "filename": "20250824_bass_Cmaj.mid",
    "track_type": "bass",
    "duration": 32.0,
    "key": "C major"
}

// Generation request
{
    "type": "generate_request",
    "track_types": ["bass", "drums"],
    "chord_progression": "C-Am-F-G",
    "tempo": 120,
    "duration": 32,
    "key": "C major"
}
```

### 2. File Management

#### Folder Structure Expected by Plugin
```
ai-band-plugin/
├── generated_accompaniments/    # Plugin monitors this folder
│   ├── bass/                   # Bass track MIDI files
│   ├── drums/                  # Drum track MIDI files
│   └── temp/                   # Temporary files during generation
├── plugin_state/               # Plugin configuration
└── logs/                       # Communication logs
```

#### File Naming Convention
```
Format: {timestamp}_{track_type}_{key}_{tempo}.mid
Examples:
- 20250824103000_bass_Cmaj_120.mid
- 20250824103005_drums_Cmaj_120.mid
```

### 3. ai-band-backend Integration

The orchestrator should coordinate with the existing ai-band-backend:

```python
# Expected backend interface
from ai_band_backend import ChordDetector, MidiGenerator

# Generation workflow the orchestrator should implement
def generate_accompaniment(audio_file, track_types=["bass", "drums"]):
    # 1. Use backend for chord detection
    detector = ChordDetector()
    chords = detector.detect_chords(audio_file)
    
    # 2. Generate MIDI for each track type
    generator = MidiGenerator()
    midi_files = []
    
    for track_type in track_types:
        midi_data = generator.generate_track(
            chords=chords,
            track_type=track_type,
            tempo=120  # From DAW or user input
        )
        
        # 3. Save to plugin's monitored folder
        filename = save_midi_file(midi_data, track_type)
        midi_files.append(filename)
    
    # 4. Notify plugin via WebSocket
    notify_plugin_new_files(midi_files)
    
    return midi_files
```

### 4. Real-time Synchronization

#### DAW Transport Sync
```python
class TransportSync:
    def __init__(self, plugin_endpoint):
        self.plugin_endpoint = plugin_endpoint
        self.last_position = 0.0
        
    async def sync_with_daw(self):
        # Plugin sends position updates
        position = await self.get_plugin_position()
        
        # Coordinate with backend generation
        if self.needs_new_content(position):
            await self.trigger_generation()
            
    def needs_new_content(self, position):
        # Logic to determine when to generate new content
        # Based on song position, remaining content, etc.
        return position > self.last_position + 32.0  # 32 beats ahead
```

## Plugin Configuration

### Default Settings
```json
{
    "orchestrator_url": "http://localhost:8000",
    "websocket_url": "ws://localhost:8000/ws/sync",
    "monitor_folder": "./generated_accompaniments",
    "auto_load_files": true,
    "sync_interval_ms": 100,
    "max_file_age_seconds": 3600,
    "supported_formats": ["mid", "midi"],
    "track_types": ["bass", "drums", "keys", "guitar"]
}
```

### Plugin States
```cpp
enum PluginState {
    Disconnected,    // No orchestrator connection
    Connected,       // Connected but idle
    Generating,      // Requesting new content
    Playing,         // Active playback
    Syncing          // Synchronizing with DAW
};
```

## Testing Integration

### Test Scenarios for Orchestrator

1. **Basic Connection Test**
   ```python
   # Test plugin can connect to orchestrator
   def test_plugin_connection():
       orchestrator = AIBandOrchestrator()
       assert orchestrator.register_plugin("test_plugin")
       assert orchestrator.get_plugin_status("test_plugin") == "connected"
   ```

2. **MIDI Generation Flow**
   ```python
   # Test complete generation workflow
   def test_midi_generation():
       # Upload audio file
       response = orchestrator.upload_audio("test_song.wav")
       
       # Request generation
       job_id = orchestrator.generate_accompaniment(
           track_types=["bass", "drums"],
           tempo=120
       )
       
       # Wait for completion
       files = orchestrator.wait_for_generation(job_id)
       
       # Verify files are in plugin folder
       assert len(files) == 2
       assert all(os.path.exists(f) for f in files)
   ```

3. **Real-time Sync Test**
   ```python
   # Test transport synchronization
   def test_transport_sync():
       orchestrator.start_transport()
       time.sleep(1.0)
       
       position = orchestrator.get_transport_position()
       assert position > 0.0
       
       orchestrator.stop_transport()
       assert orchestrator.get_transport_position() == 0.0
   ```

## Error Handling

### Expected Error Scenarios
```python
class OrchestratorErrors:
    # Connection errors
    PLUGIN_NOT_FOUND = "Plugin not registered"
    CONNECTION_TIMEOUT = "Connection timeout"
    
    # Generation errors
    GENERATION_FAILED = "Backend generation failed"
    INVALID_AUDIO_FORMAT = "Unsupported audio format"
    
    # File errors
    FILE_NOT_FOUND = "MIDI file not found"
    FOLDER_NOT_WRITABLE = "Cannot write to plugin folder"
    
    # Sync errors
    TRANSPORT_SYNC_LOST = "Lost synchronization with DAW"
    INVALID_POSITION = "Invalid transport position"
```

## Performance Requirements

### Latency Targets
- Plugin registration: < 100ms
- MIDI generation trigger: < 50ms
- File notification: < 10ms
- Transport sync update: < 5ms

### Throughput Requirements
- Support 10+ concurrent plugin instances
- Handle 100+ MIDI files in monitoring folder
- Process position updates at 10Hz minimum

## Security Considerations

### Authentication
```python
# Plugin authentication
def authenticate_plugin(plugin_id, secret_key):
    # Verify plugin is authorized
    return jwt.encode({"plugin_id": plugin_id}, secret_key)

# API security
@requires_auth
def generate_accompaniment(request):
    # Validate request comes from authenticated plugin
    pass
```

### File Security
- Restrict file access to designated folders
- Validate MIDI file integrity
- Implement file size limits
- Clean up temporary files

## Deployment Configuration

### Docker Setup
```yaml
# Example orchestrator deployment
services:
  ai-band-orchestrator:
    image: ai-band/orchestrator:latest
    ports:
      - "8000:8000"
      - "8001:8001"  # WebSocket
    volumes:
      - ./generated_files:/app/generated_files
      - ./plugin_config:/app/config
    environment:
      - BACKEND_URL=http://ai-band-backend:5000
      - PLUGIN_FOLDER=/app/generated_files
```

## Summary for AI Agent

When creating the ai-band-orchestrator:

1. **Implement REST API** with endpoints listed above
2. **Add WebSocket support** for real-time synchronization
3. **Integrate with ai-band-backend** for MIDI generation
4. **Implement file management** with folder monitoring
5. **Add transport synchronization** for DAW coordination
6. **Include error handling** for robust operation
7. **Support multiple plugin instances** concurrently
8. **Provide configuration options** for different setups

The orchestrator acts as the **bridge** between the AI backend (chord detection + MIDI generation) and the plugin (real-time playback), enabling seamless AI-powered accompaniment generation in any DAW environment.

Key success metrics:
- Plugin can connect and receive MIDI files
- Real-time transport stays synchronized
- Generation requests complete within acceptable time
- Multiple plugins can connect simultaneously
- System handles errors gracefully

Use this specification to create a production-ready orchestrator that enables the complete AI Band ecosystem.
