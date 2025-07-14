#include "PluginProcessor.h"
#include "PluginEditor.h"

VstPluginAudioProcessor::VstPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Initialize synth with 8 voices
    synth.clearVoices();
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new SineWaveVoice());

    // Add a basic sine wave sound
    synth.clearSounds();
    synth.addSound(new SineWaveSound());
}

VstPluginAudioProcessor::~VstPluginAudioProcessor() {}

void VstPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) 
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void VstPluginAudioProcessor::releaseResources() {}

void VstPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, 
                                         juce::MidiBuffer& midiMessages) 
{
    for (const auto metadata : midiMessages) {
        auto message = metadata.getMessage();
        if (message.isNoteOn()) {
            lastNotePlayed = juce::String(message.getNoteNumber()) + 
                           " (vel: " + juce::String(message.getVelocity()) + ")";
            DBG("Note On: " << message.getNoteNumber() 
                << " Velocity: " << message.getVelocity());
        }
        else if (message.isPitchWheel()) {
            currentPitchBend = message.getPitchWheelValue();
        }
        else if (message.isController() && message.getControllerNumber() == 1) {
            currentModulation = message.getControllerValue();
        }
    }
    
    // Clear the buffer first
    buffer.clear();
    
    // Process MIDI and generate sound
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* VstPluginAudioProcessor::createEditor()
{
    return new VstPluginAudioProcessorEditor(*this);
}

void VstPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {}

void VstPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VstPluginAudioProcessor();
}

// Sine Wave Sound implementation
SineWaveSound::SineWaveSound() {}
SineWaveSound::~SineWaveSound() {}

bool SineWaveSound::appliesToNote(int /*midiNoteNumber*/) { return true; }
bool SineWaveSound::appliesToChannel(int /*midiChannel*/) { return true; }

// Sine Wave Voice implementation
SineWaveVoice::SineWaveVoice() : currentAngle(0), level(0), tailOff(0) {}

bool SineWaveVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SineWaveSound*>(sound) != nullptr;
}

void SineWaveVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = 0.0;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}

void SineWaveVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        if (tailOff == 0.0)
            tailOff = 1.0;
    }
    else
    {
        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void SineWaveVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (angleDelta != 0.0)
    {
        if (tailOff > 0.0)
        {
            while (--numSamples >= 0)
            {
                auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;

                tailOff *= 0.99;

                if (tailOff <= 0.005)
                {
                    clearCurrentNote();
                    angleDelta = 0.0;
                    break;
                }
            }
        }
        else
        {
            while (--numSamples >= 0)
            {
                auto currentSample = (float)(std::sin(currentAngle) * level);

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;
            }
        }
    }
} 