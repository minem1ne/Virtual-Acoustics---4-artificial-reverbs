#include "dsp/delay.h"

#include "./mix-matrix.h"

#include <cstdlib>
//#include "JuceHeader.h"

struct UserParameters
{
	float dry{ 0.0f }, wet{ 0.0f }, decay{ 0.0f }, lowDamping{ 0.0f }, midDamping{ 0.0f }, highDamping{ 0.0f };
};

inline UserParameters updateUserParameters(juce::AudioProcessorValueTreeState& apvts)
{
	UserParameters userParams;

	userParams.dry = apvts.getRawParameterValue("dry")->load();
	userParams.wet = apvts.getRawParameterValue("wet")->load();
	userParams.decay = apvts.getRawParameterValue("decay")->load();
	//userParams.lowDamping = apvts.getRawParameterValue("lowDamping")->load();
	//userParams.midDamping = apvts.getRawParameterValue("midDamping")->load();
	//userParams.highDamping = apvts.getRawParameterValue("highDamping")->load();
	

	return userParams;
}

inline double randomInRange(double low, double high) {
	// Randomizer
	double unitRand = rand()/double(RAND_MAX);
	return low + unitRand*(high - low);
}

// Delay from signalsmith
using Delay = signalsmith::delay::Delay<double, signalsmith::delay::InterpolatorNearest>;

template<int channels=8>
struct MultiChannelMixedFeedback {

	//juce::AudioProcessorValueTreeState& apvts;
	juce::dsp::IIR::Filter<float> lowShelfFilter;
	juce::dsp::IIR::Filter<float> midShelfFilter;
	juce::dsp::IIR::Filter<float> highShelfFilter;

	using Array = std::array<double, channels>;
	std::array<int, channels> delaySamples;
	std::array<Delay, channels> delays;

	double delayMs = 150;
	double decayGain = 0.85;
	
	
	void configure(double sampleRate) {
		lowShelfFilter.reset();
		midShelfFilter.reset();
		highShelfFilter.reset();
		// Low shelf filter with cutoff at 500 Hz
		lowShelfFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 400.0, 0.7071, 0.7);

		// Mid shelf filter with center frequency at 1000 Hz and Q factor to overlap with other shelves
		midShelfFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 2000.0, 0.7071, 0.5);

		// High shelf filter with cutoff at 2000 Hz
		highShelfFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 6000.0, 0.7071, 0.3);
		double delaySamplesBase = delayMs*0.001*sampleRate;
		for (int c = 0; c < channels; ++c) {
			double r = c*1.0/channels;
			delaySamples[c] = std::pow(2, r)*delaySamplesBase;
			delays[c].resize(delaySamples[c] + 1);
			delays[c].reset();
		}
	}
	
	Array process(Array input) {

		
		Array delayed;
		for (int c = 0; c < channels; ++c) {
			input[c] = lowShelfFilter.processSample(input[c]);
			input[c] = midShelfFilter.processSample(input[c]);
			input[c] = highShelfFilter.processSample(input[c]);
			delayed[c] = delays[c].read(delaySamples[c]);
		}
		
		// Mix using a Householder matrix
		Array mixed = delayed;
		Householder<double, channels>::inPlace(mixed.data());

		// Update user Input
		//UserParameters params = updateUserParameters(apvts);
		//decayGain = params.decay;
		
		for (int c = 0; c < channels; ++c) {
			double sum = input[c] + mixed[c]*decayGain;
			delays[c].write(sum);
		}
		
		return delayed;
	}
};

template<int channels=8>
struct DiffusionStep {
	using Array = std::array<double, channels>;
	double delayMsRange = 50;
	
	std::array<int, channels> delaySamples;
	std::array<Delay, channels> delays;
	std::array<bool, channels> flipPolarity;
	
	void configure(double sampleRate) {
		double delaySamplesRange = delayMsRange*0.001*sampleRate;
		for (int c = 0; c < channels; ++c) {
			double rangeLow = delaySamplesRange*c/channels;
			double rangeHigh = delaySamplesRange*(c + 1)/channels;
			delaySamples[c] = randomInRange(rangeLow, rangeHigh);
			delays[c].resize(delaySamples[c] + 1);
			delays[c].reset();
			flipPolarity[c] = rand()%2;
		}
	}
	
	Array process(Array input) {
		// Delay
		Array delayed;
		for (int c = 0; c < channels; ++c) {
			delays[c].write(input[c]);
			delayed[c] = delays[c].read(delaySamples[c]);
		}
		
		// Mix with a Hadamard matrix
		Array mixed = delayed;
		Hadamard<double, channels>::inPlace(mixed.data());

		// Flip some polarities
		for (int c = 0; c < channels; ++c) {
			if (flipPolarity[c]) mixed[c] *= -1;
		}

		return mixed;
	}
};



template<int channels=8, int stepCount=4>
struct DiffuserHalfLengths {
	using Array = std::array<double, channels>;

	using Step = DiffusionStep<channels>;
	std::array<Step, stepCount> steps;

	DiffuserHalfLengths(double diffusionMs) {
		for (auto &step : steps) {
			diffusionMs *= 0.5;
			step.delayMsRange = diffusionMs;
		}
	}
	
	void configure(double sampleRate) {
		for (auto &step : steps) step.configure(sampleRate);
	}
	
	Array process(Array samples) {
		for (auto &step : steps) {
			samples = step.process(samples);
		}
		return samples;
	}
};

template<int channels=8, int diffusionSteps=4>
struct BasicReverb {
	using Array = std::array<double, channels>;
	
	MultiChannelMixedFeedback<channels> feedback;
	DiffuserHalfLengths<channels, diffusionSteps> diffuser;
	double dry, wet;
	juce::AudioProcessorValueTreeState& apvts;


	BasicReverb(double roomSizeMs, double rt60, juce::AudioProcessorValueTreeState& s, double dry = 0, double wet = 1) : diffuser(roomSizeMs), apvts(s), dry(dry), wet(wet) {
		feedback.delayMs = roomSizeMs;

		// How long does our signal take to go around the feedback loop?
		double typicalLoopMs = roomSizeMs*1.5;
		// How many times will it do that during our RT60 period?
		double loopsPerRt60 = rt60/(typicalLoopMs*0.001);
		// This tells us how many dB to reduce per loop
		double dbPerCycle = -60/loopsPerRt60;

		feedback.decayGain = std::pow(10, dbPerCycle*0.05);
	}
	
	void configure(double sampleRate) {
		feedback.configure(sampleRate);
		diffuser.configure(sampleRate);
		
	}
	
	Array process(Array input) {
		Array diffuse = diffuser.process(input);
		//Array diffuse = input;

		UserParameters params = updateUserParameters(apvts);
		feedback.decayGain = params.decay;
		Array longLasting = feedback.process(diffuse);
		Array output;

		//Get user parameter
		//UserParameters userParams = updateUserParameters(apvts);
		dry = params.dry;
		wet = params.wet;

		for (int c = 0; c < channels; ++c) {
			output[c] = dry*input[c] + wet*longLasting[c];
		}
		return output;
	}
};
