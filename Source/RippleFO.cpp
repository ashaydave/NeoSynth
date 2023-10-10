/*
  ==============================================================================

    RippleFO.cpp
    Created: 15 Sep 2023 9:31:03pm
    Author:  soura

  ==============================================================================
*/

#include "RippleFO.h"
#

RippleFO::RippleFO()
{


}

RippleFO::~RippleFO()
{

}

void RippleFO::prepareToPlay(float sampleRate)
{
    this->sampleRate = sampleRate;
    inc = 0.0f;
    buff = 0.0f;

}

float RippleFO::getLfoVal(int fOsc)
{
    inc = fOsc / sampleRate;
    buff = buff + inc;
    if (buff > 1.0f)
        buff = 0.0f;
    
    return (2 * buff) - 1;
}