/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VAR_2024_ReverbAudioProcessor::VAR_2024_ReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

VAR_2024_ReverbAudioProcessor::~VAR_2024_ReverbAudioProcessor()
{
}

//==============================================================================
const juce::String VAR_2024_ReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VAR_2024_ReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VAR_2024_ReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VAR_2024_ReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VAR_2024_ReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VAR_2024_ReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VAR_2024_ReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VAR_2024_ReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VAR_2024_ReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void VAR_2024_ReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VAR_2024_ReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    myReverb.configure(sampleRate);
    iirFilter.reset();
    iirFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 1000.0);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // Prepare convolution Buffer
    convolutionBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);

    convolution.prepare(spec);

    // Load the impulse response file
    auto dir = juce::File::getCurrentWorkingDirectory();
    
    int numTries = 0;

    while (!dir.getChildFile("Resources").exists() && numTries++ < 15)
        dir = dir.getParentDirectory();
    
    // Load the impulse response into the convolution processor
    convolution.loadImpulseResponse(dir.getChildFile("Resources").getChildFile("30x35yFull.wav"),
    juce::dsp::Convolution::Stereo::no,
    juce::dsp::Convolution::Trim::no,
    1024,
    juce::dsp::Convolution::Normalise::yes
    );
    
    
}

void VAR_2024_ReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VAR_2024_ReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VAR_2024_ReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
 

    for (int channel = 1; channel < 2; ++channel)
    {
        
        auto* channelData = buffer.getWritePointer(channel);
        auto* channelData0 = buffer.getWritePointer(0);
        auto bufferLength = buffer.getNumSamples();
        std::array<double, 8> inputArr{};
        std::array<double, 8> outputArr{};

        constexpr double scaling = 1.0 / 8.0;

        // Ensure the convolutionBuffer is the same size as the input buffer
        if (convolutionBuffer.getNumChannels() != buffer.getNumChannels() || convolutionBuffer.getNumSamples() != buffer.getNumSamples())
        {
            convolutionBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples());
        }

     
        convolutionBuffer.copyFrom(channel, 0, buffer.getReadPointer(channel), buffer.getNumSamples());
        
        
        for (int i = 0; i < bufferLength; ++i) // Go through the buffer
        {
            //channelData[i] = iirFilter.processSample(channelData[i]);
            for (int j = 0; j < inputArr.size(); ++j) // Go through the reverb channels
            {
               
                auto value = channelData[i];
                value = value * 1.0; // Scaling down input level
                inputArr[j] = value; // Split sample to all channels
            }
            outputArr = myReverb.process(inputArr); // Process Audio into reverb
            double sum = 0;

            for (int k = 0; k < outputArr.size(); ++k) // Mix down channels to 1 channel
            {
                sum = outputArr[k] + sum;
            }
            sum = sum * scaling;
            channelData[i] = sum;

            // Clear 1st channel
            channelData0[i] = 0;

        }
     
        ConvolutionParameters params = updateConvolutionParameters();
        // Process Early Reflections
        juce::dsp::AudioBlock<float> audioBlock(convolutionBuffer);
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);

        // Convolve the audio stream with the loaded impulse response
        convolution.process(context);

        auto gain = params.earlyReflections;
        buffer.addFrom(channel, 0, convolutionBuffer.getReadPointer(channel), convolutionBuffer.getNumSamples(), gain);
    
        // Copy to first channel
       
        //buffer.addFrom(0, 0, convolutionBuffer.getReadPointer(channel), convolutionBuffer.getNumSamples(), gain);
        buffer.addFrom(0, 0, buffer.getReadPointer(channel), buffer.getNumSamples(), 1);
    }
   
}

//==============================================================================
bool VAR_2024_ReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VAR_2024_ReverbAudioProcessor::createEditor()
{
    //return new VAR_2024_ReverbAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void VAR_2024_ReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VAR_2024_ReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ConvolutionParameters VAR_2024_ReverbAudioProcessor::updateConvolutionParameters()
{
    ConvolutionParameters convoParams;

    // Paramter
    convoParams.earlyReflections = apvts.getRawParameterValue("earlyReflections")->load();


    return convoParams;
}

juce::AudioProcessorValueTreeState::ParameterLayout VAR_2024_ReverbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Bow Paramter

    layout.add(std::make_unique<juce::AudioParameterFloat>("dry", "dry", 0.0f, 1.0f, 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("wet", "wet", 0.0f, 1.0f, 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("earlyReflections", "earlyReflections", 0.0f, 1.0f, 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("decay", "decay", 0.0f, 1.0f, 1.0f));

    //layout.add(std::make_unique<juce::AudioParameterFloat>("lowDamping", "lowDamping", 0.0f, 1.0f, 1.0f));

    //layout.add(std::make_unique<juce::AudioParameterFloat>("midDamping", "midDamping", 0.0f, 1.0f, 1.0f));

    //layout.add(std::make_unique<juce::AudioParameterFloat>("highDamping", "highDamping", 0.0f, 1.0f, 1.0f));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VAR_2024_ReverbAudioProcessor();
}
