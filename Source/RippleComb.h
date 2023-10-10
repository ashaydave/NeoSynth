/*
  ==============================================================================

    CombFilter.h
    Created: 15 Sep 2023 9:22:03pm
    Author:  soura

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RippleFO.h"

class RippleComb
{
public:
    RippleComb();
    ~RippleComb();

    void prepareToPlay(float sampleRate, int samplesPerBlock, int numInputChannels);
    void processBlock(juce::AudioBuffer<float>& buffer,int cutOff,float bGain,float aGain,int combType,int lfoFreq,bool combLfoOn,float combMod);



private:

    std::vector<float> delayBuffer;

    int readIndex, writeIndex, delayBufferLength;
    RippleFO lfo;
};