#pragma once
#ifndef M_LEXIVERB_24
#define M_LEXIVERB_24


#ifndef AUDIO_SAMPLERATE_HZ
#define AUDIO_SAMPLERATE_HZ 48000
#endif

#include "../utility/OnePoleFilter.hpp"


#include "daisy_seed.h"
#include "daisysp.h"

daisysp::DelayLine<float, ((size_t)(0.1f * 48000.0f))> DSY_SDRAM_BSS preDelay[2];
daisysp::DelayLine<float, ((size_t)(0.1f * 48000.0f))> DSY_SDRAM_BSS preDiffusionAllpass[2][4];
daisysp::DelayLine<float, ((size_t)(0.3f * 48000.0f))> DSY_SDRAM_BSS networkDelay[4];
daisysp::DelayLine<float, ((size_t)(0.1f * 48000.0f))> DSY_SDRAM_BSS networkAllpass[4];
daisysp::DelayLine<float, ((size_t)(0.1f * 48000.0f))> DSY_SDRAM_BSS earlyRefAllpass[4];

EnvelopeFollower reverbEnvelopeFollower[2];

namespace mmm{

class LexiVerb
{
  private:
    // Predelay und Prediffusion
    m::OnePoleFilter preFilter[2];

    float preDiffusionAllpassDelay[2][4] = {{4.771, 3.595, 12.73, 9.307}, {4.771, 3.595, 12.73, 9.307}};
    float preDiffusionAllpassCoefficient[2][4] = {{0.75, 0.75, 0.625, 0.625}, {0.75, 0.75, 0.625, 0.625}};
    float predelay_time_ms = 200;

    // Feedback Delay Network
    m::OnePoleFilter networkFilter[2];
    daisysp::Oscillator networkAllpassModulation[2];

    float networkAllpassDelay[4] = {30.51, 89.24, 22.58, 60.48};
    float networkAllpassCoefficient[4] = {0.7, 0.5, 0.7, 0.5};
    float networkDelay_time_ms[4] = {141.69, 106.28, 149.62, 125};

    float feedbackGain = 0.7;
    float mix = 1;
    float size_modifier = 1;

    // Early Reflections
    float earlyRefAllpassDelay[4] = {64.2, 11.2, 6.3, 41.2};
    float earlyRefAllpassCoefficiant[4] = {0.5, 0.5, 0.5, 0.5};

  public:
    LexiVerb()
    {
    }

    void Init()
    {

        reverbEnvelopeFollower[0].Init(constants::AUDIO_SAMPLERATE_HZ);
        reverbEnvelopeFollower[1].Init(constants::AUDIO_SAMPLERATE_HZ);

        reverbEnvelopeFollower[1].SetAttack(10);
        reverbEnvelopeFollower[1].SetDecay(10);

        // Predelay + prediffusion

        preDelay[0].Reset();
        preDelay[1].Reset();
        preDiffusionAllpass[0][0].Reset();
        preDiffusionAllpass[0][1].Reset();
        preDiffusionAllpass[0][2].Reset();
        preDiffusionAllpass[0][3].Reset();
        preDiffusionAllpass[1][0].Reset();
        preDiffusionAllpass[1][1].Reset();
        preDiffusionAllpass[1][2].Reset();
        preDiffusionAllpass[1][3].Reset();
        networkDelay[0].Reset();
        networkDelay[1].Reset();
        networkDelay[2].Reset();
        networkDelay[3].Reset();
        networkAllpass[0].Reset();
        networkAllpass[1].Reset();
        networkAllpass[2].Reset();
        networkAllpass[3].Reset();
        earlyRefAllpass[0].Reset();
        earlyRefAllpass[1].Reset();
        earlyRefAllpass[2].Reset();
        earlyRefAllpass[3].Reset();

        preFilter[0].init(15000, constants::AUDIO_SAMPLERATE_HZ);
        preFilter[1].init(15000, constants::AUDIO_SAMPLERATE_HZ);


        // Delay Network

        networkFilter[0].init(2000, constants::AUDIO_SAMPLERATE_HZ);
        networkFilter[1].init(2000, constants::AUDIO_SAMPLERATE_HZ);


        networkAllpassModulation[0].Init(constants::AUDIO_SAMPLERATE_HZ);
        networkAllpassModulation[0].SetAmp(4);
        networkAllpassModulation[0].SetFreq(0.1);
        networkAllpassModulation[0].SetWaveform(networkAllpassModulation[0].WAVE_SIN);
        networkAllpassModulation[1].Init(constants::AUDIO_SAMPLERATE_HZ);
        networkAllpassModulation[1].SetAmp(4);
        networkAllpassModulation[1].SetFreq(0.07);
        networkAllpassModulation[1].SetWaveform(networkAllpassModulation[1].WAVE_SIN);

    }

    void Process(const float &in1, const float &in2, float *out1, float *out2)
    {
        // Predelay + prediffusion
        preDelay[0].Write(in1);
        preDelay[1].Write(in2);

        float signalPredelay[2];

        signalPredelay[0] = preFilter[0].process(preDelay[0].Read(constants::AUDIO_SAMPLERATE_HZ * predelay_time_ms / 1000));
        signalPredelay[1] = preFilter[1].process(preDelay[1].Read(constants::AUDIO_SAMPLERATE_HZ * predelay_time_ms / 1000));

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                signalPredelay[i] = preDiffusionAllpass[i][j].Allpass(signalPredelay[i], constants::AUDIO_SAMPLERATE_HZ * preDiffusionAllpassDelay[i][j] / 1000, preDiffusionAllpassCoefficient[i][j]);
            }
        }

        // Delay Network

        float signalDelayNetwork_a[2];

        signalDelayNetwork_a[0] = signalPredelay[0] + networkDelay[3].Read(networkDelay_time_ms[3] * size_modifier) * feedbackGain;
        signalDelayNetwork_a[1] = signalPredelay[1] + networkDelay[1].Read(networkDelay_time_ms[1] * size_modifier) * feedbackGain;

        signalDelayNetwork_a[0] = networkAllpass[0].Allpass(signalDelayNetwork_a[0], networkAllpassModulation[0].Process() + constants::AUDIO_SAMPLERATE_HZ * networkAllpassDelay[0] / 1000, networkAllpassCoefficient[0]);
        signalDelayNetwork_a[1] = networkAllpass[2].Allpass(signalDelayNetwork_a[1], networkAllpassModulation[1].Process() + constants::AUDIO_SAMPLERATE_HZ * networkAllpassDelay[2] / 1000, networkAllpassCoefficient[2]);

        networkDelay[0].Write(signalDelayNetwork_a[0]);
        networkDelay[2].Write(signalDelayNetwork_a[1]);

        float signalDelayNetwork_b[2];
        float signalDelayNetwork_b_filtered[2];

        signalDelayNetwork_b_filtered[0] = networkFilter[0].process(networkDelay[0].Read(networkDelay_time_ms[0] * size_modifier));
        signalDelayNetwork_b_filtered[1] = networkFilter[1].process(networkDelay[2].Read(networkDelay_time_ms[2] * size_modifier));

        signalDelayNetwork_b[0] = networkAllpass[1].Allpass(signalDelayNetwork_b_filtered[0], constants::AUDIO_SAMPLERATE_HZ * networkAllpassDelay[1] / 1000, networkAllpassCoefficient[1]);
        signalDelayNetwork_b[1] = networkAllpass[3].Allpass(signalDelayNetwork_b_filtered[1], constants::AUDIO_SAMPLERATE_HZ * networkAllpassDelay[3] / 1000, networkAllpassCoefficient[3]);

        networkDelay[1].Write((signalDelayNetwork_b[0] * feedbackGain));
        networkDelay[3].Write((signalDelayNetwork_b[1] * feedbackGain));

        // Early Reflections

        float signalEarlyRefAllpass[4];
        float signalEarlyRef[2];

        signalEarlyRefAllpass[0] = earlyRefAllpass[0].Allpass(signalDelayNetwork_b_filtered[0], constants::AUDIO_SAMPLERATE_HZ * earlyRefAllpassDelay[1] / 1000, earlyRefAllpassCoefficiant[1]);
        signalEarlyRefAllpass[1] = earlyRefAllpass[1].Allpass(signalDelayNetwork_b_filtered[0], constants::AUDIO_SAMPLERATE_HZ * earlyRefAllpassDelay[2] / 1000, earlyRefAllpassCoefficiant[2]);
        signalEarlyRefAllpass[2] = earlyRefAllpass[2].Allpass(signalDelayNetwork_b_filtered[1], constants::AUDIO_SAMPLERATE_HZ * earlyRefAllpassDelay[3] / 1000, earlyRefAllpassCoefficiant[3]);
        signalEarlyRefAllpass[3] = earlyRefAllpass[3].Allpass(signalDelayNetwork_b_filtered[1], constants::AUDIO_SAMPLERATE_HZ * earlyRefAllpassDelay[4] / 1000, earlyRefAllpassCoefficiant[4]);

        signalEarlyRef[0] = networkDelay[0].Read(8.9 * size_modifier) + networkDelay[0].Read(99.8 * size_modifier);
        signalEarlyRef[0] -= signalEarlyRefAllpass[0];
        signalEarlyRef[0] += networkDelay[1].Read(67 * size_modifier);
        signalEarlyRef[0] -= networkDelay[2].Read(66.8 * size_modifier);
        signalEarlyRef[0] -= signalEarlyRefAllpass[2];
        signalEarlyRef[0] -= networkDelay[3].Read(35.8 * size_modifier);
        // signalEarlyRef[0] *= 0.125;

        signalEarlyRef[1] = networkDelay[2].Read(11.8 * size_modifier) + networkDelay[2].Read(121.7 * size_modifier);
        signalEarlyRef[1] -= signalEarlyRefAllpass[3];
        signalEarlyRef[1] += networkDelay[3].Read(89.7 * size_modifier);
        signalEarlyRef[1] -= networkDelay[0].Read(70.8 * size_modifier);
        signalEarlyRef[1] -= signalEarlyRefAllpass[1];
        signalEarlyRef[1] -= networkDelay[1].Read(4.1 * size_modifier);
        // signalEarlyRef[1] *= 0.125;


        *out1 = daisysp::SoftLimit(signalEarlyRef[0]) * mix + in1 * (-mix + 1);
        *out2 = daisysp::SoftLimit(signalEarlyRef[1]) * mix + in2 * (-mix + 1);
    }


    void SetMix(float drywet)
    {
        mix = drywet;
        mix = fmaxf(mix, 0.0f);
        mix = fminf(mix, 1.0f);
    }

    void SetPredelay(float time_ms)
    {
        predelay_time_ms = time_ms;
        predelay_time_ms = fmaxf(predelay_time_ms, 0.1f);
        predelay_time_ms = fminf(predelay_time_ms, 100.0f);
    }

    void SetSize(float size)
    {
        size = fmaxf(size, 0.1f);
        size = fminf(size, 2.0f);
        size_modifier = size;
    }

    void SetPreFilter(float freq)
    {
        freq = fmaxf(freq, 50.0f);
        freq = fminf(freq, 20000.0f);

        preFilter[0].setTime(freq);
        preFilter[1].setTime(freq);
    }

    void SetDampingFilter(float freq)
    {
        freq = fmaxf(freq, 100.0f);
        freq = fminf(freq, 20000.0f);
        networkFilter[0].setTime(freq);
        networkFilter[1].setTime(freq);
    }

    void SetFeedback(float gain)
    {
        feedbackGain = gain;
        feedbackGain = fmaxf(feedbackGain, 0.0f);
        feedbackGain = fminf(feedbackGain, 1.0f);
    }
};
} // namespace mmm

#endif