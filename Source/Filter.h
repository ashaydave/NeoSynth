/*
  ==============================================================================

    Filter.h
    Created: 4 Dec 2023 8:48:37pm
    Author:  soura

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class Filter
{
public:
    Filter();
    ~Filter();

    void prepareToPlay(float sampleRate, int samplesPerBlock, int numInputChannels);
    void processBlock(juce::AudioBuffer<float>& buffer, int cutOff, float bGain, float aGain, int combType, int lfoFreq, bool combLfoOn, float combMod);



private:

};