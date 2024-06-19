/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Reverb/reverb.h"

//==============================================================================
/**
*/

struct ConvolutionParameters
{
    float earlyReflections{ 0.0f };
};

class VAR_2024_ReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    VAR_2024_ReverbAudioProcessor();
    ~VAR_2024_ReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    ConvolutionParameters updateConvolutionParameters();

    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    double roomSizeMs = 50.0; // Example room size in milliseconds
    double rt60 = 2.5; // Example RT60 time
    double dryLevel = 1.0; // Example dry signal level
    double wetLevel = 0.0; // Example wet signal level
    juce::dsp::IIR::Filter<float> iirFilter;
    juce::dsp::Convolution convolution;
    juce::AudioBuffer<float> convolutionBuffer;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Create an object with default template arguments (channels=8, diffusionSteps=4)
    BasicReverb<8, 4> myReverb{ roomSizeMs, rt60, apvts, dryLevel, wetLevel };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VAR_2024_ReverbAudioProcessor)

};
