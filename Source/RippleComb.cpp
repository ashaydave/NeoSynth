/*
  ==============================================================================

    CombFilter.cpp
    Created: 15 Sep 2023 9:22:03pm
    Author:  soura

  ==============================================================================
*/

#include "RippleComb.h"

RippleComb::RippleComb()
{
    readIndex = 0;
    writeIndex = 0;
    delayBufferLength = 0;
}

RippleComb::~RippleComb()
{

}

void RippleComb::prepareToPlay(float sampleRate, int samplesPerBlock, int numInputChannels)
{
    delayBufferLength = 256;    //(sampleRate / samplesPerBlock);
    delayBuffer.resize(delayBufferLength*numInputChannels,0.0f);
    
    writeIndex = 0;

    lfo.prepareToPlay(sampleRate);

}

void RippleComb::processBlock(juce::AudioBuffer<float>& buffer, int cutOff, float bGain,float aGain, int combType, int lfoFreq, bool combLfoOn,float combMod)
{
    readIndex = writeIndex - cutOff;



    for (int n = 0; n < buffer.getNumSamples(); n++)
    {
        
        
        for (int channel = 0; channel < buffer.getNumChannels(); channel++)

        {
            if (readIndex < 0)
                readIndex = delayBufferLength + readIndex;
            float dryVal = buffer.getReadPointer(channel)[n];
            
            switch (combType)
            {
            case 0: 
                buffer.getWritePointer(channel)[n] = bGain * buffer.getReadPointer(channel)[n] + delayBuffer[readIndex];
                delayBuffer[writeIndex] = -bGain * dryVal + aGain * buffer.getReadPointer(channel)[n];
                break;
             case 1: 
                buffer.getWritePointer(channel)[n] = bGain * buffer.getReadPointer(channel)[n] + delayBuffer[readIndex];
                delayBuffer[writeIndex] = bGain * dryVal - aGain * buffer.getReadPointer(channel)[n];
                break;
            case 2:
                buffer.getWritePointer(channel)[n] = bGain * buffer.getReadPointer(channel)[n] + delayBuffer[readIndex];
                delayBuffer[writeIndex] = bGain * dryVal + aGain * buffer.getReadPointer(channel)[n];
                break;
            case 3:
                buffer.getWritePointer(channel)[n] = bGain * buffer.getReadPointer(channel)[n] + delayBuffer[readIndex];
                delayBuffer[writeIndex] = -bGain * dryVal - aGain * buffer.getReadPointer(channel)[n];
                break;
            
            
            }
            
            if (combLfoOn)
                readIndex = readIndex + combMod * (lfo.getLfoVal(lfoFreq));
            
            readIndex = (readIndex + 1) % delayBufferLength;
            
            writeIndex = (writeIndex + 1) % delayBufferLength;
            if (readIndex == writeIndex)
            {
                readIndex = writeIndex - 1;
                if (readIndex < 0)
                    readIndex = delayBufferLength + readIndex;
            }
                


        }
    }

}