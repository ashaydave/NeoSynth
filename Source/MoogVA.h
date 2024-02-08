/*
  ==============================================================================

    MoogVA.h
    Created: 1 Nov 2023 12:54:08pm
    Author:  soura

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class MoogVA
{
public:
    MoogVA();
    ~MoogVA();

    void prepareToPlay(float sampleRate, int samplesPerBlock, int numInputChannels);
    void processBlock(juce::AudioBuffer<float>& buffer,float cutoffFrequency,float resonance,float bassComp,bool antiAliasing);

    std::vector<float> antiImageFilt(std::vector<float> input);
    void lowShelf(juce::AudioBuffer<float>& buffer);
    void updateLowShelfCoefficients(float cutOff, float gain);
private:

    float fs, fs2;
    juce::AudioBuffer<float> oversampledBuffer;
    std::vector<float> oversampledBuffer2;
    std::vector<float> delayTaps;
    float Gcomp = 0.5;  //Compensation of Passband Gain.
    float g,h0,h1;
    std::vector<float> w, wold; //State Variables

    float xnm1 = 0.0f, xnm2 = 0.0f, ynm1 = 0.0f, ynm2 = 0.0f;//Delay Memory for Low Shelf
    float a0, a1,  b1, c0; //Coefficients for low shelf
};