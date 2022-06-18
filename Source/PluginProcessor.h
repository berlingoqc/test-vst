#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class AudioPluginAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor()
        : AudioProcessor (BusesProperties().withInput  ("Input",     juce::AudioChannelSet::stereo())           // [1]
                                           .withOutput ("Output",    juce::AudioChannelSet::stereo())
                                           .withInput  ("Sidechain", juce::AudioChannelSet::stereo()))
    {
        addParameter (threshold = new juce::AudioParameterFloat ("threshold", "Gain", 0.0f, 1.0f, 0.5f));  // [2]
        addParameter (alpha     = new juce::AudioParameterFloat ("alpha",     "Alpha",     0.0f, 1.0f, 0.8f));
    }

    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        // the sidechain can take any layout, the main bus needs to be the same on the input and output
        return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet()
                 && ! layouts.getMainInputChannelSet().isDisabled();
    }

    //==============================================================================
    void prepareToPlay (double, int) override
    {
        lowPassCoeff = 0.0f;    // [3]
        sampleCountDown = 0;    // [4]
    }

    void releaseResources() override          {}

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
    {
        buffer.applyGain(threshold->get());
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override            { return new juce::GenericAudioProcessorEditor (*this); }
    bool hasEditor() const override                                { return true; }
    const juce::String getName() const override                    { return "Noise Gate"; }
    bool acceptsMidi() const override                              { return false; }
    bool producesMidi() const override                             { return false; }
    double getTailLengthSeconds() const override                   { return 0.0; }
    int getNumPrograms() override                                  { return 1; }
    int getCurrentProgram() override                               { return 0; }
    void setCurrentProgram (int) override                          {}
    const juce::String getProgramName (int) override               { return {}; }
    void changeProgramName (int, const juce::String&) override     {}

    bool isVST2() const noexcept                                   { return (wrapperType == wrapperType_VST); }

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override
    {
        juce::MemoryOutputStream stream (destData, true);

        stream.writeFloat (*threshold);
        stream.writeFloat (*alpha);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        juce::MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);

        threshold->setValueNotifyingHost (stream.readFloat());
        alpha->setValueNotifyingHost (stream.readFloat());
    }

private:
    //==============================================================================
    juce::AudioParameterFloat* threshold;
    juce::AudioParameterFloat* alpha;
    int sampleCountDown;

    float lowPassCoeff;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
