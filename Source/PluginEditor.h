#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class VstPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private juce::MidiKeyboardState::Listener,
                                    private juce::Timer
{
public:
    explicit VstPluginAudioProcessorEditor(VstPluginAudioProcessor&);
    ~VstPluginAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    // MidiKeyboardState::Listener
    void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

private:
    VstPluginAudioProcessor& processorRef;
    juce::Label lastNoteLabel;
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    
    juce::Slider pitchBendSlider;
    juce::Slider modulationSlider;
    juce::Label pitchBendLabel;
    juce::Label modulationLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VstPluginAudioProcessorEditor)
}; 