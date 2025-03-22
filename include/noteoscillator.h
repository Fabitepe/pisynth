#ifndef NOTEOSCILLATOR_H
#define NOTEOSCILLATOR_H

#include <vector>
#include <cstddef>

#include "oscillator.h"
#include "notes.h"

class NoteOscillator: public Oscillator
{

public:
    NoteOscillator(
        Waveform waveform = Sine,
        int sampleRate = 48000,
        float attack = 1.0f,
        float decay = 600.0f,
        float sustain = 0.7f,
        float release = 600.0f
    );
    void setAttack(float attack);
    void setDecay(float decay);
    void setSustain(float sustain);
    void setRelease(float release);
    void setOctaveOffset(int offset);
    void triggerNote(float frequency, float volume);
    void triggerNote(Note note, int octave, float volume);
    void endNote();
    float nextSample();

private:

    float attackStage();
    float decayStage();
    float releaseStage();

    enum EnvelopeStage {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    NoteOscillator::EnvelopeStage stage;
    int noteSampleCounter;

    float attack;     // ms
    float decay;      // ms
    float sustain;    // 0 to 1
    float release;    // ms

    int octaveOffset;
};

#endif // NOTEOSCILLATOR_H
