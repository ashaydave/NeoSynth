/*
  ==============================================================================

    RippleFO.h
    Created: 15 Sep 2023 9:31:03pm
    Author:  soura

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class RippleFO
{
public:
    RippleFO();
    ~RippleFO();

    void prepareToPlay(float sampleRate);
    float getLfoVal(int fOsc);



private:
    float buff;
    float sampleRate;
    float inc;
};