/*
  ==============================================================================

    VirtualKeyboardComponent.h
    Created: 30 Sep 2023 3:31:21pm
    Author:  soura

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class VirtualKeyboardComponent  : public juce::Component,
    private juce::MidiInputCallback,
    private juce::MidiKeyboardStateListener
{
public:
    VirtualKeyboardComponent();
    ~VirtualKeyboardComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void VirtualKeyboardComponent::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    //void VirtualKeyboardComponent::handleNoteOn(juce::MidiKeyboardState* keyState, int midiChannel, int midiNoteNumber, float velocity) override;
   void handleNoteOff(juce::MidiKeyboardState* , int midiChannel, int midiNoteNumber, float /*velocity*/) override;
   void handleNoteOn(juce::MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/) override;
private:

    juce::MidiKeyboardState keyboardState;            
    juce::MidiKeyboardComponent keyboardComponent;
    double startTime;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualKeyboardComponent)
};
