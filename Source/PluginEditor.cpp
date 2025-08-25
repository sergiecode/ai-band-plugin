#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AIBandAudioProcessorEditor::AIBandAudioProcessorEditor (AIBandAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), directoryThread("Directory Scanner")
{
    // Start the directory scanning thread
    directoryThread.startThread(3); // Lower priority
    
    // Set up playback controls
    playButton.setButtonText("Play");
    playButton.addListener(this);
    addAndMakeVisible(playButton);
    
    stopButton.setButtonText("Stop");
    stopButton.addListener(this);
    addAndMakeVisible(stopButton);
    
    loadButton.setButtonText("Load Files...");
    loadButton.addListener(this);
    addAndMakeVisible(loadButton);
    
    // Set up status displays
    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(statusLabel);
    
    positionLabel.setText("Position: 0.0", juce::dontSendNotification);
    positionLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(positionLabel);
    
    tempoLabel.setText("Tempo: 120 BPM", juce::dontSendNotification);
    tempoLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(tempoLabel);
    
    // Set up folder selection
    folderButton.setButtonText("Select MIDI Folder...");
    folderButton.addListener(this);
    addAndMakeVisible(folderButton);
    
    folderLabel.setText("No folder selected", juce::dontSendNotification);
    folderLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(folderLabel);
    
    // Set up network settings (for future orchestrator integration)
    networkGroup.setText("Orchestrator Connection (Future)");
    addAndMakeVisible(networkGroup);
    
    serverLabel.setText("Server:", juce::dontSendNotification);
    addAndMakeVisible(serverLabel);
    
    serverEditor.setText("localhost");
    serverEditor.setEnabled(false); // Disabled for now
    addAndMakeVisible(serverEditor);
    
    portLabel.setText("Port:", juce::dontSendNotification);
    addAndMakeVisible(portLabel);
    
    portEditor.setText("8080");
    portEditor.setEnabled(false); // Disabled for now
    addAndMakeVisible(portEditor);
    
    connectButton.setButtonText("Connect");
    connectButton.setEnabled(false); // Disabled for now
    connectButton.addListener(this);
    addAndMakeVisible(connectButton);
    
    // Set up file browser
    setupFileBrowser();
    
    // Initialize visual feedback
    bassActive = false;
    drumActive = false;
    
    // Set plugin window size
    setSize(600, 500);
    
    // Start timer for regular updates
    startTimer(50); // 20 FPS updates
}

AIBandAudioProcessorEditor::~AIBandAudioProcessorEditor()
{
    stopTimer();
    directoryThread.stopThread(1000);
}

//==============================================================================
void AIBandAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    // Draw title
    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawFittedText ("AI Band Plugin", getLocalBounds().removeFromTop(30), 
                      juce::Justification::centred, 1);
    
    // Draw visual indicators for bass and drum activity
    auto indicatorArea = getLocalBounds().removeFromBottom(40).reduced(margin);
    auto bassArea = indicatorArea.removeFromLeft(indicatorArea.getWidth() / 2 - componentSpacing);
    auto drumArea = indicatorArea;
    
    // Bass indicator
    g.setColour(bassActive ? juce::Colours::green : juce::Colours::darkgrey);
    g.fillRect(bassArea.reduced(2));
    g.setColour(juce::Colours::white);
    g.drawText("BASS", bassArea, juce::Justification::centred);
    
    // Drum indicator
    g.setColour(drumActive ? juce::Colours::red : juce::Colours::darkgrey);
    g.fillRect(drumArea.reduced(2));
    g.setColour(juce::Colours::white);
    g.drawText("DRUMS", drumArea, juce::Justification::centred);
}

void AIBandAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop(30);
    auto indicatorArea = area.removeFromBottom(40);
    area.reduce(margin, margin);
    
    // Playback controls section
    auto controlsArea = area.removeFromTop(buttonHeight + componentSpacing);
    auto buttonWidth = controlsArea.getWidth() / 3 - componentSpacing;
    
    playButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    controlsArea.removeFromLeft(componentSpacing);
    stopButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    controlsArea.removeFromLeft(componentSpacing);
    loadButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    
    area.removeFromTop(componentSpacing);
    
    // Status section
    auto statusArea = area.removeFromTop(labelHeight * 3 + componentSpacing * 2);
    statusLabel.setBounds(statusArea.removeFromTop(labelHeight));
    statusArea.removeFromTop(componentSpacing);
    positionLabel.setBounds(statusArea.removeFromTop(labelHeight));
    statusArea.removeFromTop(componentSpacing);
    tempoLabel.setBounds(statusArea.removeFromTop(labelHeight));
    
    area.removeFromTop(componentSpacing);
    
    // Folder selection section
    auto folderArea = area.removeFromTop(buttonHeight + labelHeight + componentSpacing);
    folderButton.setBounds(folderArea.removeFromTop(buttonHeight));
    folderArea.removeFromTop(componentSpacing);
    folderLabel.setBounds(folderArea.removeFromTop(labelHeight));
    
    area.removeFromTop(componentSpacing);
    
    // Network settings section
    auto networkArea = area.removeFromTop(120);
    networkGroup.setBounds(networkArea);
    networkArea.reduce(10, 20); // Inset for group box
    
    auto serverRow = networkArea.removeFromTop(buttonHeight);
    serverLabel.setBounds(serverRow.removeFromLeft(60));
    serverEditor.setBounds(serverRow.reduced(componentSpacing, 0));
    
    networkArea.removeFromTop(componentSpacing);
    
    auto portRow = networkArea.removeFromTop(buttonHeight);
    portLabel.setBounds(portRow.removeFromLeft(60));
    portEditor.setBounds(portRow.removeFromLeft(80));
    portRow.removeFromLeft(componentSpacing);
    connectButton.setBounds(portRow);
    
    area.removeFromTop(componentSpacing);
    
    // File browser (remaining space)
    if (fileBrowser != nullptr)
    {
        fileBrowser->setBounds(area);
    }
}

void AIBandAudioProcessorEditor::timerCallback()
{
    updateDisplay();
}

void AIBandAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &playButton)
    {
        audioProcessor.startPlayback();
        statusLabel.setText("Playing", juce::dontSendNotification);
    }
    else if (button == &stopButton)
    {
        audioProcessor.stopPlayback();
        statusLabel.setText("Stopped", juce::dontSendNotification);
    }
    else if (button == &loadButton)
    {
        loadSelectedFiles();
    }
    else if (button == &folderButton)
    {
        chooseMidiFolder();
    }
    else if (button == &connectButton)
    {
        // Future: Connect to orchestrator
        updateNetworkStatus();
    }
}

void AIBandAudioProcessorEditor::selectionChanged()
{
    // Update load button state based on selection
    if (fileBrowser != nullptr)
    {
        bool hasSelection = fileBrowser->getNumSelectedFiles() > 0;
        loadButton.setEnabled(hasSelection);
    }
}

void AIBandAudioProcessorEditor::fileClicked (const juce::File& file, const juce::MouseEvent& e)
{
    juce::ignoreUnused(file, e);
    // Handle file click if needed
}

void AIBandAudioProcessorEditor::fileDoubleClicked (const juce::File& file)
{
    if (file.hasFileExtension(".mid") || file.hasFileExtension(".midi"))
    {
        // Auto-load on double-click
        auto filename = file.getFileName().toLowerCase();
        juce::String bassFile, drumFile;
        
        if (filename.contains("bass"))
            bassFile = file.getFullPathName();
        else if (filename.contains("drum"))
            drumFile = file.getFullPathName();
        else
        {
            // If unclear, treat as bass file
            bassFile = file.getFullPathName();
        }
        
        audioProcessor.loadMidiFiles(bassFile, drumFile);
        statusLabel.setText("Loaded: " + file.getFileName(), juce::dontSendNotification);
    }
}

void AIBandAudioProcessorEditor::browserRootChanged (const juce::File& newRoot)
{
    juce::ignoreUnused(newRoot);
    // Handle root change if needed
}

//==============================================================================
// Internal methods

void AIBandAudioProcessorEditor::updateDisplay()
{
    // Update playback position
    auto currentBeat = audioProcessor.getCurrentBeat();
    positionLabel.setText("Position: " + juce::String(currentBeat, 1) + " beats", 
                         juce::dontSendNotification);
    
    // Update play/stop button states
    bool isPlaying = audioProcessor.isPlaying();
    playButton.setEnabled(!isPlaying);
    stopButton.setEnabled(isPlaying);
    
    // Update visual indicators (simulate activity based on playback)
    bassActive = isPlaying && (static_cast<int>(currentBeat) % 2 == 0);
    drumActive = isPlaying && (static_cast<int>(currentBeat * 2) % 2 == 1);
    
    // Trigger repaint for visual indicators
    repaint(getLocalBounds().removeFromBottom(40));
}

void AIBandAudioProcessorEditor::setupFileBrowser()
{
    // Set up file filter for MIDI files
    fileFilter.reset(new juce::WildcardFileFilter("*.mid;*.midi", "*", "MIDI Files"));
    
    // Set up directory list
    directoryList.reset(new juce::DirectoryContentsList(fileFilter.get(), directoryThread));
    directoryList->setDirectory(juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), 
                               true, true);
    
    // Create file browser
    fileBrowser.reset(new juce::FileBrowserComponent(juce::FileBrowserComponent::openMode |
                                                    juce::FileBrowserComponent::canSelectFiles |
                                                    juce::FileBrowserComponent::canSelectMultipleItems,
                                                    juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                                    fileFilter.get(),
                                                    nullptr));
    
    fileBrowser->addListener(this);
    addAndMakeVisible(fileBrowser.get());
}

void AIBandAudioProcessorEditor::loadSelectedFiles()
{
    if (fileBrowser == nullptr)
        return;
        
    auto selectedFiles = fileBrowser->getSelectedFiles();
    if (selectedFiles.isEmpty())
        return;
    
    juce::String bassFile, drumFile;
    
    // Identify bass and drum files from selection
    for (auto& file : selectedFiles)
    {
        if (!file.hasFileExtension(".mid") && !file.hasFileExtension(".midi"))
            continue;
            
        auto filename = file.getFileName().toLowerCase();
        if (filename.contains("bass") && bassFile.isEmpty())
            bassFile = file.getFullPathName();
        else if (filename.contains("drum") && drumFile.isEmpty())
            drumFile = file.getFullPathName();
    }
    
    // If we couldn't identify specific files, use the first two
    if (bassFile.isEmpty() && drumFile.isEmpty() && selectedFiles.size() >= 1)
    {
        bassFile = selectedFiles[0].getFullPathName();
        if (selectedFiles.size() >= 2)
            drumFile = selectedFiles[1].getFullPathName();
    }
    
    // Load the files
    if (bassFile.isNotEmpty() || drumFile.isNotEmpty())
    {
        bool success = audioProcessor.loadMidiFiles(bassFile, drumFile);
        if (success)
        {
            statusLabel.setText("Files loaded successfully", juce::dontSendNotification);
        }
        else
        {
            statusLabel.setText("Error loading files", juce::dontSendNotification);
        }
    }
}

void AIBandAudioProcessorEditor::chooseMidiFolder()
{
    auto folderChooser = std::make_unique<juce::FileChooser>("Select MIDI Folder",
                                                            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                                            "");
    
    auto chooserFlags = juce::FileBrowserComponent::openMode | 
                       juce::FileBrowserComponent::canSelectDirectories;
    
    folderChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser)
    {
        auto results = chooser.getResults();
        if (results.size() > 0)
        {
            auto folder = results[0];
            audioProcessor.setMidiFolder(folder.getFullPathName());
            folderLabel.setText("Monitoring: " + folder.getFullPathName(), juce::dontSendNotification);
            
            // Update file browser to show the selected folder
            if (fileBrowser != nullptr && directoryList != nullptr)
            {
                directoryList->setDirectory(folder, true, true);
                fileBrowser->setRoot(folder);
            }
        }
    });
}

void AIBandAudioProcessorEditor::updateNetworkStatus()
{
    // Future: Update connection status with orchestrator
    // For now, just show that it's not implemented
    statusLabel.setText("Network features coming soon", juce::dontSendNotification);
}
