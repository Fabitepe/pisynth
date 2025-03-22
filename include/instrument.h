#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <vector>
#include <cstddef>

#include "notes.h"
#include "noteoscillator.h"

class Instrument
{
public:
    Instrument();

    void addOscillator(NoteOscillator* osc, int octave_offset = 0, float volume = 1.0f);
    void triggerNote(Note note, unsigned int octave, float volume = 1.0f);
    void endNote();
    float nextSample();

private:
    std::vector<NoteOscillator*> oscillators;
};


#endif //INSTRUMENT_H
