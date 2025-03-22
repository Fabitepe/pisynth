#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <vector>
#include <cstddef>

#include "notes.h"


class Oscillator 
{
public:
    enum Waveform 
    {
        Sine,
        Square,
        Saw,
        Triangle
    };

    Oscillator(Waveform waveform = Sine, int sampleRate = 48000);
    
    void setFrequency(float freq);
    void setWaveform(Waveform wf);
    void setVol(float v);
    void addOffset(float offset);
    float nextSample();
    std::vector<float> getNextSamples(size_t frameCount);

protected:

    float getNoteFrequency(Note note, unsigned int octave);

    Waveform waveform;
    float sampleRate;
    float frequency;
    float vol;

    float phase;
    float phaseIncrement;

    void advancePhase();
};

#endif // OSCILLATOR_H
