#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
    AI Band Plugin Editor
    
    Created by Sergie Code - Software Engineer & Programming Educator
    Part of the AI Band Ecosystem for musicians
    YouTube: https://www.youtube.com/@SergieCode
    GitHub: https://github.com/sergiecode
    
    This class provides the user interface for the AI Band Plugin.
    It allows users to control playback, load MIDI files, and configure settings.
*/
class AIBandAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer,
                                   public juce::Button::Listener,
                                   public juce::FileBrowserListener
{
public:
    AIBandAudioProcessorEditor (AIBandAudioProcessor&);
    ~AIBandAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void buttonClicked (juce::Button* button) override;
    
    // FileBrowserListener
    void selectionChanged() override;
    void fileClicked (const juce::File& file, const juce::MouseEvent& e) override;
    void fileDoubleClicked (const juce::File& file) override;
    void browserRootChanged (const juce::File& newRoot) override;

private:
    // Reference to processor
    AIBandAudioProcessor& audioProcessor;
    
    //==============================================================================
    // UI Components
    
    // Playback controls
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton loadButton;
    
    // Status display
    juce::Label statusLabel;
    juce::Label positionLabel;
    juce::Label tempoLabel;
    
    // File browser
    std::unique_ptr<juce::FileBrowserComponent> fileBrowser;
    juce::TimeSliceThread directoryThread;
    std::unique_ptr<juce::DirectoryContentsList> directoryList;
    std::unique_ptr<juce::WildcardFileFilter> fileFilter;
    
    // Settings
    juce::TextButton folderButton;
    juce::Label folderLabel;
    
    // Network settings (for future orchestrator integration)
    juce::GroupComponent networkGroup;
    juce::Label serverLabel;
    juce::TextEditor serverEditor;
    juce::Label portLabel;
    juce::TextEditor portEditor;
    juce::TextButton connectButton;
    
    // Visual feedback
    juce::Rectangle<int> bassIndicator;
    juce::Rectangle<int> drumIndicator;
    bool bassActive;
    bool drumActive;
    
    //==============================================================================
    // Layout constants
    static constexpr int margin = 10;
    static constexpr int buttonHeight = 30;
    static constexpr int labelHeight = 20;
    static constexpr int componentSpacing = 5;
    
    //==============================================================================
    // Internal methods
    void updateDisplay();
    void setupFileBrowser();
    void loadSelectedFiles();
    void chooseMidiFolder();
    void updateNetworkStatus();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AIBandAudioProcessorEditor)
};
