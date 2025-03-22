#include "noteoscillator.h"

#include <iostream>


NoteOscillator::NoteOscillator(
    Waveform waveform,
    int sampleRate,
    float attack,
    float decay,
    float sustain,
    float release
) : Oscillator(waveform, sampleRate), stage(EnvelopeStage::Idle), noteSampleCounter(0), attack(attack), decay(decay), sustain(sustain), release(release), octaveOffset(0)
{
}

void NoteOscillator::setAttack(float attack) { this->attack = attack; }

void NoteOscillator::setDecay(float decay) { this->decay = decay; }

void NoteOscillator::setSustain(float sustain) { this->sustain = sustain; }

void NoteOscillator::setRelease(float release) { this->release = release; }

void NoteOscillator::setOctaveOffset(int offset) { this->octaveOffset = offset; }


float downCurveValue(float progress)
{
    float x = progress - 1.0f;
    float value = x * x;
    if(progress >= 1.0f)
    {
        return 0.0f;
    }
    return value;
}


float NoteOscillator::attackStage()
{
    int samplesPerMS = this->sampleRate / 1000;
    int attackSamples = samplesPerMS * this->attack;

    if(this->noteSampleCounter > attackSamples)
    {
        this->stage = EnvelopeStage::Decay;
        return 1.0f;
    }
    float vol = ((float) this->noteSampleCounter) / ((float) attackSamples);
    return vol;
}


float NoteOscillator::decayStage()
{
    int samplesPerMS = this->sampleRate / 1000;
    int attackSamples = samplesPerMS * this->attack;
    int decaySamples = samplesPerMS * this->decay;
    int lastDecaySample = attackSamples + decaySamples;

    if(this->noteSampleCounter > lastDecaySample)
    {
        if(this->sustain > 0.0f)
        {
            this->stage = EnvelopeStage::Sustain;
            return this->sustain;
        }else
        {
            this->stage = EnvelopeStage::Idle;
            this->phase = 0.0f;
            return 0.0;
        }
    }

    float progress = ((float) (this->noteSampleCounter - attackSamples)) / ((float) decaySamples);
    float decayCurveValue = downCurveValue(1.0f - progress);
    float vol = 1.0f - ((1.0f - this->sustain) * decayCurveValue);
    return vol;
}


float NoteOscillator::releaseStage()
{
    int samplesPerMS = this->sampleRate / 1000;
    int releaseSamples = samplesPerMS * this->release;
    float progress = (float) this->noteSampleCounter / (float) releaseSamples;
    float releaseCurveValue = downCurveValue(progress);
    float stageVolume = this->sustain * releaseCurveValue;

    if(stageVolume <= 0.0f)
    {
        this->stage = EnvelopeStage::Idle;
        this->phase = 0.0f;
        stageVolume = 0.0f;
    }
    return stageVolume;
}


float NoteOscillator::nextSample()
{
    if(stage == EnvelopeStage::Idle)
    {
        return 0.0;
    }

    float sample = Oscillator::nextSample();
    float stageVolume = 1.0;

    switch(this->stage)
    {
    case EnvelopeStage::Attack:
        stageVolume = this->attackStage();
        break;
        
    case EnvelopeStage::Decay:
        stageVolume = this->decayStage();
        break;

    case EnvelopeStage::Sustain:
        stageVolume = this->sustain;
        break;

    case EnvelopeStage::Release:
        stageVolume = this->releaseStage();

    default:
        break;
    }

    this->noteSampleCounter++;
    return sample * stageVolume;
}


void NoteOscillator::triggerNote(float frequency, float volume)
{
    this->phase = 0.0f;
    this->setFrequency(frequency);
    this->noteSampleCounter = 0;
    this->stage = EnvelopeStage::Attack;
}


void NoteOscillator::triggerNote(Note note, int octave, float volume)
{
    this->triggerNote(Oscillator::getNoteFrequency(note, octave + this->octaveOffset), volume);
}


void NoteOscillator::endNote()
{
    this->stage = EnvelopeStage::Release;
    this->noteSampleCounter = 0;
}
