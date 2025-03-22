#include <cmath>

#include "oscillator.h"


Oscillator::Oscillator(Waveform waveform, int sampleRate): waveform(waveform), sampleRate(sampleRate), phase(0.0f), frequency(440.0f), vol(1.0f)
{
    this->setFrequency(frequency);
}


void Oscillator::setFrequency(float freq)
{
    this->frequency = freq;
    this->phaseIncrement = (2.0f * M_PI * frequency) / sampleRate;
}


void Oscillator::setWaveform(Waveform wf)
{
    this->waveform = wf;
}


void Oscillator::setVol(float v)
{
    this->vol = v;
}


void Oscillator::addOffset(float offset)
{
    while(offset > 2.0f * M_PI)
    {
        offset -= 2.0f * M_PI;
    }
    
    this->phase += offset;
    if (this->phase > 2.0f * M_PI)
    {
        this->phase -= 2.0f * M_PI;
    }
}


std::vector<float> Oscillator::getNextSamples(size_t frameCount)
{
    std::vector<float> samples(frameCount);
    
    for (size_t i = 0; i < frameCount; i++) {
        samples[i] = this->nextSample();
    }

    return samples;
}


float Oscillator::nextSample()
{
    float sample_value = 0.0f;
    switch (this->waveform) {
        case Sine:
            sample_value = sin(this->phase);
            break;
        case Square:
            if(sin(this->phase) >= 0.0f)
            {
                sample_value = 1.0f;
            }else
            {
                sample_value = -1.0f;
            }
            break;
        case Saw:
            sample_value = (2.0f * (this->phase / (2.0f * M_PI))) - 1.0f;
            break;
        case Triangle:
            sample_value = 2.0f * fabs(2.0f * (this->phase / (2.0f * M_PI)) - 1.0f) - 1.0f;
            break;
        default:
            sample_value = 0.0f;
            break;
    }
    this->advancePhase();
    return sample_value * this->vol;
}


void Oscillator::advancePhase()
{
    this->phase += this->phaseIncrement;
    if (this->phase > 2.0f * M_PI)
    {
        this->phase -= 2.0f * M_PI;
    }
}


float Oscillator::getNoteFrequency(Note note, unsigned int octave)
{
    switch(note)
    {
    case Note::A:
        return TABLE_A[octave];
    case Note::Ais:
        return TABLE_Ais[octave];
    case Note::B:
        return TABLE_B[octave];
    case Note::C:
        return TABLE_C[octave];
    case Note::Cis:
        return TABLE_Cis[octave];
    case Note::D:
        return TABLE_D[octave];
    case Note::Dis:
        return TABLE_Dis[octave];
    case Note::E:
        return TABLE_E[octave];
    case Note::F:
        return TABLE_F[octave];
    case Note::Fis:
        return TABLE_Fis[octave];
    case Note::G:
        return TABLE_G[octave];
    case Note::Gis:
        return TABLE_Gis[octave];

    default:
        return TABLE_C[2];
    }
}