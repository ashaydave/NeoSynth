/*
  ==============================================================================

    Oscillator.cpp
    Created: 30 Sep 2023 4:01:47pm
    Author:  soura

  ==============================================================================
*/

#include "Oscillator.h"


Oscillator::Oscillator(float sampleRate, float numChannels,float samplesPerBlock,float fOsc,int noteNumber) 
{
    this->sampleRate = sampleRate;
    this->fOsc = fOsc;
    this->samplesPerBlock = samplesPerBlock;
    this->numChannels = numChannels;
    this->noteNumber = noteNumber;
    buff = 0.0f;
    inc = 0.0f;
    
    onOff = 1.0f;
}

Oscillator::~Oscillator()
{

}

void Oscillator::prepareToPlay(float sampleRate, int samplesPerBlock, int numOutputChannels)
{
    this->sampleRate = sampleRate;
    buff = 0.0f;
    inc = 0.0f;
    fOsc = 0.0f;
    onOff = 0.0f;
}

void Oscillator::processBlock(juce::AudioBuffer<float>& buffer)
{

    for (int sample = 0; sample < buffer.getNumSamples();sample++)
    {
        inc = fOsc / sampleRate;
        buff += inc;
        //buff = buff + inc;
        if (buff > 1)
            buff = buff-1;

        for (int channel = 0; channel < buffer.getNumChannels(); channel++)
        {
            /*switch (type)
            {
            case 0:break;

            case 1:break;
            case 2:
            }*/
                buffer.getWritePointer(channel)[sample] = onOff * std::sin(2 * juce::MathConstants<float>::pi * buff-1);
            //buffer.getWritePointer(channel)[sample] = onOff *(2*buff - 1);
            
        }
    }
    //return buffer;

}

void Oscillator::noteOn(float fOsc)
{
    this->fOsc = fOsc;
    this->onOff = 1.0f;
}
void Oscillator::noteOff()
{
    this->onOff = 0.0f;

}