/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class VAR_2024_ReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    VAR_2024_ReverbAudioProcessorEditor (VAR_2024_ReverbAudioProcessor&);
    ~VAR_2024_ReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VAR_2024_ReverbAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VAR_2024_ReverbAudioProcessorEditor)
};
