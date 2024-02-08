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
    this->oscBuffer = juce::AudioBuffer<float>(numChannels, samplesPerBlock);
    onOff = 1.0f;
    this->prevFOsc = fOsc;
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

juce::AudioBuffer<float> Oscillator::processBlock(juce::AudioBuffer<float>& buffer,int synthType,int offSet)
{
    oscBuffer.clear();
    //if(fOsc!=fOsc+)
    for (int sample = 0; sample < oscBuffer.getNumSamples();sample++)
    {
        
        inc = fOsc / sampleRate;
        buff += inc;
        //buff = buff + inc;
        if (buff > 1)
            buff = buff-1;

        for (int channel = 0; channel < oscBuffer.getNumChannels(); channel++)
        {
            //oscBuffer.getWritePointer(channel)[sample] = ((2 * buff - 1) > 0) ? 1.0f : -1.0f;
            switch (synthType)
            {
                
            case 0:oscBuffer.getWritePointer(channel)[sample] = std::sin(2 * juce::MathConstants<float>::pi * buff - 1);
                break;
            case 1:oscBuffer.getWritePointer(channel)[sample] = (2 * buff - 1); 
                break;
            case 2:oscBuffer.getWritePointer(channel)[sample] = 2 * abs(buff * 2 - 1) - 1;
                break;
            case 3:oscBuffer.getWritePointer(channel)[sample] = ((2*buff-1)>0) ? 1.0f : -1.0f;
                break;
            }
        }
    }
    return oscBuffer;

}

void Oscillator::noteOn(float fOsc)
{
    this->fOsc = fOsc;
    this->onOff = 1.0f;
    this->prevFOsc = fOsc;
}
void Oscillator::noteOff()
{
    this->onOff = 0.0f;

}