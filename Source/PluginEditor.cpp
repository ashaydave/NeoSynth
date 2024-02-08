/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthzAudioProcessorEditor::SynthzAudioProcessorEditor (SynthzAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
    startTime(juce::Time::getMillisecondCounterHiRes() * 0.001)
    
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //addAndMakeVisible(keyboardComponent);
    //keyboardState.addListener(this);


    //addAndMakeVisible(virtualKeyboardComponent);
    
    //audioProcessor.setMidi
    setOpaque(true);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);

    
    m.addItem(1, "Sine");
    m.addItem(2, "Saw");
    m.addItem(3, "Square");
    m.addItem(4, "Triangle");

    addAndMakeVisible(oscTypeButton);
    oscTypeButton.setButtonText("Oscillator");
    //oscTypeButton.onClick([{}]);

    setSize (400, 300);
}

SynthzAudioProcessorEditor::~SynthzAudioProcessorEditor()
{
}

//==============================================================================
void SynthzAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    g.setColour(juce::Colours::hotpink);
    //g.fillPath(p);
    
    m.showMenuAsync(juce::PopupMenu::Options(), [](int result) {
        if (result == 0)
        {

        }

        });

}

void SynthzAudioProcessorEditor::resized()
{
    //virtualKeyboardComponent.setBounds(10, 10, 380, 280);
    keyboardComponent.setBounds(0,200,400,100);
}

void SynthzAudioProcessorEditor::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    //const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent(message);
    //postMessageToList(message, source->getName());
}

void SynthzAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState* keyState, int midiChannel, int midiNoteNumber, float velocity)
{

    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    //audioProcessor.oscillator.noteOn(m.getMidiNoteInHertz(m.getNoteNumber()));
    //getParentComponent().audioProcessor
    //postMessageToList(m, "On-Screen Keyboard");
   // audioProcessor.oscillators.push_back((new Oscillator(audioProcessor.getSampleRate(), audioProcessor.getNumOutputChannels(), audioProcessor.samplesPerBlock, m.getMidiNoteInHertz(m.getNoteNumber()),m.getNoteNumber())));

}

void SynthzAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState* keyState, int midiChannel, int midiNoteNumber, float /*velocity*/)
{

    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    //postMessageToList(m, "On-Screen Keyboard");
    //audioProcessor.oscillator.noteOff();
    for (int i = 0; i < audioProcessor.oscillators1.size(); i++)
    {
        //if(oscillators[it].)
        //DBG(juce::String(oscillators[i].fOsc) + " zozododo " + juce::String(currentMessage.getMidiNoteInHertz(currentMessage.getNoteNumber())));
        if (audioProcessor.oscillators1[i]->noteNumber == m.getNoteNumber())
        {
            //delete oscillators[i];

            DBG("\n\nopopopopopopoopopopopopopopopopopo\n\n");
            delete audioProcessor.oscillators1[i];
            //audioProcessor.oscillators.erase(audioProcessor.oscillators.begin() + i);
        }
    }

}
