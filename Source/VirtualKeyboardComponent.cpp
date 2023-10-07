/*
  ==============================================================================

    VirtualKeyboardComponent.cpp
    Created: 30 Sep 2023 3:31:21pm
    Author:  soura

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VirtualKeyboardComponent.h"

//==============================================================================
VirtualKeyboardComponent::VirtualKeyboardComponent():
    keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    startTime(juce::Time::getMillisecondCounterHiRes() * 0.001)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setOpaque(true);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);

    setSize(400, 300);
}

VirtualKeyboardComponent::~VirtualKeyboardComponent()
{
    keyboardState.removeListener(this);
}

void VirtualKeyboardComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll(juce::Colours::black);//getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("VirtualKeyboardComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void VirtualKeyboardComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    keyboardComponent.setBounds(100,100,300, 100);

}

void VirtualKeyboardComponent::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    //const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent(message);
    //postMessageToList(message, source->getName());
}

void VirtualKeyboardComponent::handleNoteOn(juce::MidiKeyboardState* keyState, int midiChannel, int midiNoteNumber, float velocity)
{
    
        auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);

        //getParentComponent().audioProcessor
        //postMessageToList(m, "On-Screen Keyboard");
    
}

void VirtualKeyboardComponent::handleNoteOff(juce::MidiKeyboardState* keyState, int midiChannel, int midiNoteNumber, float /*velocity*/)
{
    
        auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
        m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
        //postMessageToList(m, "On-Screen Keyboard");
    
}
