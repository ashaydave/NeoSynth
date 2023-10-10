/*
  ==============================================================================

    RippleGain.cpp
    Created: 15 Sep 2023 9:30:02pm
    Author:  soura

  ==============================================================================
*/

#include "RippleGain.h"


RippleGain::RippleGain()
{


}

RippleGain::~RippleGain()
{

}

void RippleGain::prepareToPlay(float sampleRate)
{
    lfo.prepareToPlay(sampleRate);
    gainToMult = 0.0f;

}

void RippleGain::processBlock(juce::AudioBuffer<float>& buffer,float gainDb, int lfoFreq,bool gainLfoOn,float gainMod)
{
    for (int n = 0; n < buffer.getNumSamples(); n++)
    {
        if (gainLfoOn)
        {
            float lfoZZ = lfo.getLfoVal(lfoFreq);
            gainToMult = gainDb + gainMod*lfoZZ;
        }
        else
            gainToMult = gainDb;
            

        for (int channel = 0; channel < buffer.getNumChannels(); channel++)
        {
            buffer.getWritePointer(channel)[n] = powf(10.0f, (gainToMult/ 20.0f))*buffer.getReadPointer(channel)[n];
        }
    }

}