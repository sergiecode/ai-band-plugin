#pragma once

#include <JuceHeader.h>
#include <functional>
#include <memory>

//==============================================================================
/**
    Network Client for AI Band Plugin
    
    This class provides communication capabilities with the ai-band-orchestrator.
    Currently contains placeholder functionality that will be implemented
    when the orchestrator server is available.
*/
class NetworkClient
{
public:
    //==============================================================================
    NetworkClient();
    ~NetworkClient();
    
    //==============================================================================
    /** Initialize the network client */
    void initialize();
    
    /** Shutdown and cleanup network resources */
    void shutdown();
    
    //==============================================================================
    // Connection Management
    
    /** Connect to the orchestrator server
        @param serverAddress    Server IP address or hostname
        @param port            Server port number
        @returns true if connection successful
    */
    bool connectToServer(const juce::String& serverAddress, int port);
    
    /** Disconnect from the server */
    void disconnect();
    
    /** Check if currently connected to server */
    bool isConnected() const { return connected; }
    
    /** Get current connection status */
    juce::String getConnectionStatus() const;
    
    //==============================================================================
    // Chord Progression Communication
    
    /** Send chord progression to server for AI generation
        @param chords          Array of chord data
        @param tempo           Tempo in BPM
        @param key             Musical key
        @param callback        Callback for when generation is complete
        @returns true if request was sent successfully
    */
    bool requestGeneration(const juce::Array<juce::var>& chords,
                          int tempo,
                          const juce::String& key,
                          std::function<void(bool success, const juce::String& bassFile, const juce::String& drumFile)> callback);
    
    /** Send real-time chord data for live generation
        @param chord           Current chord
        @param timestamp       Time when chord was detected
        @returns true if sent successfully
    */
    bool sendRealtimeChord(const juce::String& chord, double timestamp);
    
    //==============================================================================
    // File Management
    
    /** Request list of available generated files from server
        @param callback        Callback with file list
        @returns true if request was sent
    */
    bool requestFileList(std::function<void(const juce::StringArray& files)> callback);
    
    /** Download a generated MIDI file from server
        @param filename        Name of file to download
        @param localPath       Local path to save file
        @param callback        Callback when download complete
        @returns true if download started
    */
    bool downloadFile(const juce::String& filename,
                     const juce::String& localPath,
                     std::function<void(bool success)> callback);
    
    //==============================================================================
    // WebSocket Communication (Future)
    
    /** Enable real-time WebSocket communication */
    void enableRealtimeMode(bool enable);
    
    /** Check if real-time mode is active */
    bool isRealtimeModeEnabled() const { return realtimeMode; }
    
    //==============================================================================
    // Callback Management
    
    /** Set callback for connection status changes */
    void setConnectionCallback(std::function<void(bool connected, const juce::String& status)> callback);
    
    /** Set callback for real-time generation results */
    void setRealtimeGenerationCallback(std::function<void(const juce::String& bassData, const juce::String& drumData)> callback);
    
    /** Set callback for server notifications */
    void setNotificationCallback(std::function<void(const juce::String& message)> callback);

private:
    //==============================================================================
    // Connection state
    bool connected;
    bool realtimeMode;
    juce::String serverAddress;
    int serverPort;
    juce::String connectionStatusMessage;
    
    // Callbacks
    std::function<void(bool, const juce::String&)> connectionCallback;
    std::function<void(const juce::String&, const juce::String&)> realtimeGenerationCallback;
    std::function<void(const juce::String&)> notificationCallback;
    
    //==============================================================================
    // HTTP Communication (Placeholder)
    struct HttpRequest
    {
        juce::String url;
        juce::String method;
        juce::String body;
        juce::HashMap<juce::String, juce::String> headers;
        std::function<void(int statusCode, const juce::String& response)> callback;
    };
    
    /** Send HTTP request to server (placeholder implementation) */
    bool sendHttpRequest(const HttpRequest& request);
    
    /** Create JSON payload for chord progression */
    juce::String createChordProgressionJson(const juce::Array<juce::var>& chords, int tempo, const juce::String& key);
    
    /** Parse JSON response from server */
    juce::var parseJsonResponse(const juce::String& response);
    
    //==============================================================================
    // WebSocket Communication (Future Implementation)
    class WebSocketConnection;
    std::unique_ptr<WebSocketConnection> webSocket;
    
    /** Initialize WebSocket connection */
    bool initializeWebSocket();
    
    /** Handle incoming WebSocket messages */
    void handleWebSocketMessage(const juce::String& message);
    
    //==============================================================================
    // Background Threading
    juce::TimeSliceThread networkThread;
    
    /** Background network processing */
    void processNetworkEvents();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NetworkClient)
};
