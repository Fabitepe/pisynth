#include "instrument.h"

#include <iostream>


Instrument::Instrument()
{
}


void Instrument::addOscillator(NoteOscillator* osc, int octave_offset, float volume)
{
    osc->setVol(volume);
    osc->setOctaveOffset(octave_offset);
    this->oscillators.push_back(osc);
}


float Instrument::nextSample()
{
    float sample = 0.0f;

    for(NoteOscillator* osc: this->oscillators)
    {
        sample += osc->nextSample();
    }
    sample /= (float) this->oscillators.size();

    return sample;
}


void Instrument::triggerNote(Note note, unsigned int octave, float volume)
{
    for(NoteOscillator* osc: this->oscillators)
    {
        osc->triggerNote(note, octave, volume);
    }
}


void Instrument::endNote()
{
    for(NoteOscillator* osc: this->oscillators)
    {
        osc->endNote();
    }
}

