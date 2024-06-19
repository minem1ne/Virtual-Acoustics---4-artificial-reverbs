#if !defined(M_Allpass_24)
#define M_Allpass_24


#include "CircularBuffer.hpp"

#ifndef AUDIO_SAMPLERATE_HZ
#define AUDIO_SAMPLERATE_HZ 48000
#endif

namespace m
{
class Allpass
{
    private:

    CircularBuffer<AUDIO_SAMPLERATE_HZ *100/1000> buffer;
    float time_ms_ = 100, gain_ = 1;


    public:

    Allpass(){    }

    void SetTime(float time_ms){
        time_ms_ = time_ms;
    }

    void SetGain(float gain){
        gain_ = gain;
        gain_ = fmaxf(gain_, 0.0f);
        gain_ = fminf(gain_, 1.0f);
    }

    float Process(float input){
        float bufferOut = buffer.readBuffer(time_ms_);
        float mix = input + bufferOut * gain_;
        buffer.writeBuffer(mix);
        float output = mix * (-1 * gain_) + bufferOut;

        return output;
    }
};
} // namespace m

#endif