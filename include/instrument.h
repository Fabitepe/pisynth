#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <vector>
#include <map>
#include <cstddef>
#include <mutex>

#include "notes.h"
#include "noteoscillator.h"

class Instrument
{
public:
    Instrument(int polyphony = 10);

    void addOscillator(NoteOscillator& osc, int octave_offset = 0, float volume = 1.0f);
    void triggerNote(Note note, unsigned int octave, float volume = 1.0f);
    void endNote(Note note, unsigned int octave);
    float nextSample();

private:
    int polyphony;
    std::mutex lock;
    std::vector<NoteOscillator> oscillators;
    std::map<unsigned int, std::vector<NoteOscillator*>*> playingOscillators;

};


#endif //INSTRUMENT_H
