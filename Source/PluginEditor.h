/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//#include "VirtualKeyboardComponent.h"

//==============================================================================
/**
*/
class SynthzAudioProcessorEditor  : public juce::AudioProcessorEditor,
    private juce::MidiInputCallback,
    private juce::MidiKeyboardStateListener
    
{
public:
    SynthzAudioProcessorEditor (SynthzAudioProcessor&);
    ~SynthzAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override;
    void handleNoteOn(juce::MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/) override;
    
private:

    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    double startTime;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SynthzAudioProcessor& audioProcessor;
    //VirtualKeyboardComponent virtualKeyboardComponent;
    juce::Path p;
    juce::PopupMenu m;
    juce::TextButton oscTypeButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthzAudioProcessorEditor)
};
