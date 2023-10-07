/*
  ==============================================================================

    Oscillator.h
    Created: 30 Sep 2023 4:01:47pm
    Author:  soura

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class Oscillator
{
public:
    Oscillator(float sampleRate, float numChannels, float samplesPerBlock, float fOsc,int noteNumber);
    ~Oscillator();

    void prepareToPlay(float sampleRate, int samplesPerBlock, int numOutputChannels);
    juce::AudioBuffer<float> processBlock(juce::AudioBuffer<float>& buffer);
    void noteOn(float fOsc);
    void noteOff();

    float sampleRate, buff, inc, fOsc, onOff;
    int noteNumber;
    
private:
    //float sampleRate, buff, inc, fOsc, onOff;
    float numChannels, samplesPerBlock;
    juce::AudioBuffer<float> oscBuffer;
};