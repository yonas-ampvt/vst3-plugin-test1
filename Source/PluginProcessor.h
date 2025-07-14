#pragma once
#include <JuceHeader.h>

// Define Sine Wave Sound
class SineWaveSound : public juce::SynthesiserSound
{
public:
    SineWaveSound();
    ~SineWaveSound() override;

    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;
};

// Define Sine Wave Voice
class SineWaveVoice : public juce::SynthesiserVoice
{
public:
    SineWaveVoice();

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newValue) override {}
    void controllerMoved(int controllerNumber, int newValue) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
};

class VstPluginAudioProcessorEditor;  // Forward declaration

class VstPluginAudioProcessor : public juce::AudioProcessor {
public:
    VstPluginAudioProcessor();
    ~VstPluginAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    int getCurrentPitchBend() const { return currentPitchBend; }
    int getCurrentModulation() const { return currentModulation; }

private:
    juce::String lastNotePlayed;
    int currentPitchBend = 8192;
    int currentModulation = 0;
    
    juce::Synthesiser synth;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VstPluginAudioProcessor)
}; 