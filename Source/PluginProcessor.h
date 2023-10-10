/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Oscillator.h"
#include "RippleComb.h"
#include "RippleGain.h"
//==============================================================================
/**
*/
class SynthzAudioProcessor  : public foleys::MagicProcessor,public juce::MidiKeyboardStateListener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SynthzAudioProcessor();
    ~SynthzAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    //juce::AudioProcessorEditor* createEditor() override;
    //bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    //juce::ValueTree createGuiValueTree() override;
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

        //void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override;
    void handleNoteOn(juce::MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/) override;
    
    //==============================================================================
   // void getStateInformation (juce::MemoryBlock& destData) override;
    //void setStateInformation (const void* data, int sizeInBytes) override;
    //Oscillator oscillator;
    std::vector<Oscillator*> oscillators;
    float samplesPerBlock=0.0f;
    juce::AudioBuffer<float> tempBuffer;
    void processMidiOn(juce::MidiMessage);
    void processMidiOff(juce::MidiMessage);
private:
    //==============================================================================
    foleys::MagicPlotSource* analyzer = nullptr;
    foleys::MagicLevelSource* levelMeter=nullptr;


    //juce::MidiKeyboardStateListener stateListener;
    juce::MidiKeyboardState keyboardState;
    //FX

    RippleComb combFilter;
    RippleGain gainMultiplier;

    //Parameters
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<float>* synthType = nullptr;

    std::atomic<float>* cutOff = nullptr;
    std::atomic<float>* resA = nullptr;
    std::atomic<float>* resB = nullptr;

    std::atomic<float>* gainDb = nullptr;
    std::atomic<float>* combOn = nullptr;
    std::atomic<float>* combLfoOn = nullptr;
    std::atomic<float>* gainLfoOn = nullptr;
    std::atomic<float>* lfoFreq = nullptr;

    std::atomic<float>* combType = nullptr;


    std::atomic<float>* gainMod = nullptr;
    std::atomic<float>* combMod = nullptr;

    //juce::MidiKeyboardState &midiKeyBoardState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthzAudioProcessor)
};
