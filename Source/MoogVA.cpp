/*
  ==============================================================================

    MoogVA.cpp
    Created: 1 Nov 2023 12:54:08pm
    Author:  soura

  ==============================================================================
*/

#include "MoogVA.h"

MoogVA::MoogVA()
{

}

MoogVA::~MoogVA()
{

}

void MoogVA::prepareToPlay(float sampleRate, int samplesPerBlock, int numInputChannels)
{
    fs = sampleRate;
    fs2 = 2 * sampleRate;
    g = 0.0f;
    w.resize(5, 0.0f);
    wold.resize(5, 0.0f);
}

void MoogVA::processBlock(juce::AudioBuffer<float>& buffer,float cutoffFrequency,float resonance,float bassComp,bool antiAliasing)
{

    updateLowShelfCoefficients(800, bassComp/4);//LowShelf for bass compensation
    lowShelf(buffer);
    lowShelf(buffer);
    lowShelf(buffer);
    lowShelf(buffer);
    
    oversampledBuffer.setSize(1, 2 * buffer.getNumSamples(), false, false, false);
    oversampledBuffer2.resize(2 * buffer.getNumSamples(), 0.0f);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        oversampledBuffer2[2 * sample] = buffer.getReadPointer(0)[sample];
        
    }

        oversampledBuffer2 = antiImageFilt(oversampledBuffer2);
        g = 2 * juce::MathConstants<float>::pi * cutoffFrequency / fs2;
        h0 = g / 1.3;
        h1 = g * 0.3 / 1.3;

    for (int i= 0; i< oversampledBuffer2.size(); i++)
    {
      //  buffer.copyFrom(channel, 0, oversampledBuffer2, buffer.getNumSamples());
        float u = oversampledBuffer2[i] - 4 * resonance * (wold[4] - Gcomp * oversampledBuffer2[i]);//Input & Feedback
        w[0] = tanh(u);
        w[1] = h0 * w[0] + h1 * wold[0] + (1 - g) * wold[1];//First IIR
        w[2] = h0 * w[1] + h1 * wold[1] + (1 - g) * wold[2];//Second IIR
        w[3] = h0 * w[2] + h1 * wold[2] + (1 - g) * wold[3];//Third IIR
        w[4] = h0 * w[3] + h1 * wold[3] + (1 - g) * wold[4];//Fourth IIR

        oversampledBuffer2[i] = w[4]; //Filter OutPut
        wold = w; //Move Unit delays
    }
    if(antiAliasing)
        oversampledBuffer2 = antiImageFilt(oversampledBuffer2); //Antialiasing Filter
    //
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        buffer.getWritePointer(0)[sample] = oversampledBuffer2[sample * 2];
    }

    for (int channel = 1; channel < buffer.getNumChannels(); channel++)
    {
        buffer.copyFrom(channel, 0,buffer.getReadPointer(0),buffer.getNumSamples(),1.0f);
    }
    //buffer.copyFrom(1, 0, buffer.getReadPointer(0), buffer.getNumSamples());



}


std::vector<float> MoogVA::antiImageFilt(std::vector<float> input)
{
    delayTaps.resize(10, 0.0f);
    float currDel;
    float a1 = juce::nextFloatDown(powf(-3.250615490734487, -18));
    float a3 = juce::nextFloatDown(powf(1.321043449104680, -17));
    
    for (int i = 0; i < input.size(); i++)
    {
        currDel= input[i];
        input[i] = 0.0051 * input[i] + a1 * delayTaps[0] - 0.0419 * delayTaps[1]
            + a3 * delayTaps[2] + 0.2885 * delayTaps[3] + 0.4968 * delayTaps[4]
            + 0.2885 * delayTaps[5] + a3 * delayTaps[6] - 0.0419 * delayTaps[7]
            + a1 * delayTaps[8] + 0.0051 * delayTaps[9];
        
        
        delayTaps[9] = delayTaps[8];
        delayTaps[8] = delayTaps[7];
        delayTaps[7] = delayTaps[6];
        delayTaps[6] = delayTaps[5];
        delayTaps[5] = delayTaps[4];
        delayTaps[4] = delayTaps[3];
        delayTaps[3] = delayTaps[2];
        delayTaps[2] = delayTaps[1];
        delayTaps[1] = delayTaps[0];
        delayTaps[0] = currDel;
        
    }
    return input;
}

void MoogVA::lowShelf(juce::AudioBuffer<float>& buffer)
{

   // updateLowShelfCoefficients(cutOff,gain);
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        float temp = buffer.getReadPointer(0)[i];
        buffer.getWritePointer(0)[i] = buffer.getReadPointer(0)[i]+c0*(a0 * buffer.getReadPointer(0)[i] + a1 * xnm1  - b1 * ynm1 );
        
        xnm1 = temp;
        ynm1 = buffer.getReadPointer(0)[i];

    }
}

void::MoogVA::updateLowShelfCoefficients(float cutOff, float gain)
{
    float thetac = 2 * juce::MathConstants<float>::pi * cutOff / fs;
    float meu = powf(10, gain / 20);
    float beta = 4 / (1 + meu);
    float del = beta * tan(thetac / 2);
    float gama = (1 - del) / (1 + del);
    a0 = (1 - gama) / 2;
    a1= (1 - gama) / 2;
    
    b1 = -1*gama;
    c0 = meu - 1;
}