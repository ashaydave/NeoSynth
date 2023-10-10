/*
  ==============================================================================

    RippleGain.h
    Created: 15 Sep 2023 9:30:02pm
    Author:  soura

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RippleFO.h"

class RippleGain
{
public:
    RippleGain();
    ~RippleGain();

    void prepareToPlay(float sampleRate);
    void processBlock(juce::AudioBuffer<float>& buffer,float gainDb,int lfoFreq,bool gainLfoOn,float gainMod);



private:
    RippleFO lfo;
    float gainToMult;
};
