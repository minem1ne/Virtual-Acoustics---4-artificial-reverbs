#if !defined(M_CircularBuffer_24)
#define M_CircularBuffer_24

#include <cmath>


namespace m
{
template<size_t BufferSize>
class CircularBuffer
{
public:
    /*
    construct like this:

    m::CircularBuffer<AUDIO_SAMPLERATE_HZ * Buffer_time_ms /1000> DSY_SDRAM_BSS preDelay;
    */
    CircularBuffer()
    {

    }


    void writeBuffer(float sample)
    {
        buffer_[writePtr_] = sample;

        writePtr_ = (writePtr_ + 1) % BufferSize;
    }


    float readBuffer(float msDelay)
    {
        size_t delaySamples = static_cast<size_t>(std::round((msDelay / 1000.0f) * sampleRate_));
        size_t readPos = (readPtr_ + delaySamples) % BufferSize;

        return buffer_[readPos];
    }

private:
    float sampleRate_; // Der sampleRate_ Wert muss weiterhin bekannt sein, um readBuffer zu verwenden
    size_t readPtr_ = 0;
    size_t writePtr_ = 0;
    float buffer_[BufferSize] = {0};
};
} // namespace m
#endif
