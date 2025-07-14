#include "PluginProcessor.h"
#include "PluginEditor.h"

VstPluginAudioProcessorEditor::VstPluginAudioProcessorEditor(VstPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), 
      processorRef(p),
      keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(600, 500);
    
    addAndMakeVisible(lastNoteLabel);
    lastNoteLabel.setText("Waiting for MIDI...", juce::dontSendNotification);
    lastNoteLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);

    // Setup pitch bend slider
    addAndMakeVisible(pitchBendSlider);
    pitchBendSlider.setRange(0, 16383, 1);
    pitchBendSlider.setValue(8192);
    pitchBendSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    addAndMakeVisible(pitchBendLabel);
    pitchBendLabel.setText("Pitch Bend", juce::dontSendNotification);
    pitchBendLabel.setJustificationType(juce::Justification::centred);

    // Setup modulation slider
    addAndMakeVisible(modulationSlider);
    modulationSlider.setRange(0, 127, 1);
    modulationSlider.setValue(0);
    modulationSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    addAndMakeVisible(modulationLabel);
    modulationLabel.setText("Modulation", juce::dontSendNotification);
    modulationLabel.setJustificationType(juce::Justification::centred);

    startTimerHz(30); // Update UI 30 times per second
}

VstPluginAudioProcessorEditor::~VstPluginAudioProcessorEditor()
{
    keyboardState.removeListener(this);
    stopTimer();
}

void VstPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void VstPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    lastNoteLabel.setBounds(area.removeFromTop(50));
    
    auto controlArea = area.removeFromTop(100);
    auto pitchBendArea = controlArea.removeFromLeft(controlArea.getWidth() / 2);
    auto modulationArea = controlArea;
    
    pitchBendLabel.setBounds(pitchBendArea.removeFromTop(20));
    pitchBendSlider.setBounds(pitchBendArea.reduced(10, 10));
    
    modulationLabel.setBounds(modulationArea.removeFromTop(20));
    modulationSlider.setBounds(modulationArea.reduced(10, 10));
    
    keyboardComponent.setBounds(area.removeFromBottom(100));
}

void VstPluginAudioProcessorEditor::timerCallback()
{
    // Update sliders to reflect current values
    pitchBendSlider.setValue(processorRef.getCurrentPitchBend(), juce::dontSendNotification);
    modulationSlider.setValue(processorRef.getCurrentModulation(), juce::dontSendNotification);
}

void VstPluginAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, 
                                               int midiNoteNumber, float velocity)
{
    lastNoteLabel.setText("Note On: " + juce::String(midiNoteNumber) + 
                         " (vel: " + juce::String(int(velocity * 127)) + ")" +
                         " [ch: " + juce::String(midiChannel + 1) + "]",
                         juce::dontSendNotification);
    keyboardComponent.setMidiChannel(midiChannel + 1);
}

void VstPluginAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, 
                                                int midiNoteNumber, float velocity)
{
    lastNoteLabel.setText("Note Off: " + juce::String(midiNoteNumber),
                         juce::dontSendNotification);
} 