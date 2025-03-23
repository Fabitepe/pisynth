#include "instrument.h"

#include <iostream>


Instrument::Instrument(int polyphony): polyphony(polyphony)
{
}


void Instrument::addOscillator(NoteOscillator& osc, int octave_offset, float volume)
{
    const std::lock_guard<std::mutex> lg(this->lock);
    osc.setVol(volume);
    osc.setOctaveOffset(octave_offset);
    this->oscillators.push_back(osc);
}


float Instrument::nextSample()
{
    const std::lock_guard<std::mutex> lg(this->lock);
    if(this->playingOscillators.size() == 0)
    {
        return 0.0f;
    }
    float sample = 0.0f;
    std::vector<unsigned int> doneNoteValues;
    
    for(std::map<unsigned int, std::vector<NoteOscillator*>*>::iterator it = this->playingOscillators.begin(); it != this->playingOscillators.end(); ++it)
    {
        unsigned int noteValue = it->first;
        bool allDone = true;

        for(int i = 0; i < it->second->size(); i++)
        {
            if(!it->second->at(i)->isDone())
            {
                allDone = false;
            }
            sample += it->second->at(i)->nextSample();
        }
        if(allDone)
        {
            doneNoteValues.push_back(noteValue);
        }
    }
    
    for(unsigned int note: doneNoteValues)
    {
        for(int i = 0; i < this->playingOscillators[note]->size(); i++)
        {
            NoteOscillator* osc = this->playingOscillators[note]->at(i);
            delete osc;
        }
        delete this->playingOscillators[note];
        this->playingOscillators.erase(note);
    }
    
    sample /= ((float) (this->polyphony * this->oscillators.size()));

    return sample;
}


void Instrument::triggerNote(Note note, unsigned int octave, float volume)
{
    const std::lock_guard<std::mutex> lg(this->lock);
    unsigned int noteValue = (unsigned int) note + octave * 12;
    if(this->playingOscillators.find(noteValue) != this->playingOscillators.end())
    {
        for(int i = 0; i < this->oscillators.size(); i++)
        {
            this->playingOscillators[noteValue]->at(i)->triggerNote(note, octave, volume);
        }
        return;
    }

    if(this->playingOscillators.size() >= this->polyphony)
    {
        return;
    }
    std::vector<NoteOscillator*>* list = new std::vector<NoteOscillator*>();
    this->playingOscillators[noteValue] = list;
    for(int i = 0; i < this->oscillators.size(); i++)
    {
        NoteOscillator* osc = new NoteOscillator(this->oscillators[i]);
        osc->triggerNote(note, octave, volume);
        this->playingOscillators[noteValue]->push_back(osc);
    }
}


void Instrument::endNote(Note note, unsigned int octave)
{
    const std::lock_guard<std::mutex> lg(this->lock);
    unsigned int noteValue = (unsigned int) note + octave * 12;
    if(this->playingOscillators.find(noteValue) == this->playingOscillators.end())
    {
        return;
    }
    this->playingOscillators[noteValue];
    for(int i = 0; i < this->playingOscillators[noteValue]->size(); i++)
    {
        this->playingOscillators[noteValue]->at(i)->endNote();
    }
}

