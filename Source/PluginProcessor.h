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
#include "BitcrusherData.h"
#include "DelayData.h"
#include "GetAGripverb.h"
#include "MoogVA.h"


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
    

    void combToggle();
    //==============================================================================
   // void getStateInformation (juce::MemoryBlock& destData) override;
    //void setStateInformation (const void* data, int sizeInBytes) override;
    //Oscillator oscillator;
    std::vector<Oscillator*> oscillators1;
    std::vector<Oscillator*> oscillators2;
    std::vector<Oscillator*> oscillators3;
    float samplesPerBlock=0.0f;
    juce::AudioBuffer<float> tempBuffer1;
    juce::AudioBuffer<float> tempBuffer2;
    juce::AudioBuffer<float> tempBuffer3;
    void processMidiOn(juce::MidiMessage);
    void processMidiOff(juce::MidiMessage);
private:
    //==============================================================================
    foleys::MagicPlotSource* analyzer = nullptr;
    foleys::MagicLevelSource* levelMeter=nullptr;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    //juce::MidiKeyboardStateListener stateListener;
    juce::MidiKeyboardState keyboardState;
    //FX

    RippleComb combFilter;
    RippleGain gainMultiplier;
    MoogVA moogVA;

    //Parameters
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<float>* synthType1 = nullptr;
    std::atomic<float>* synthType2 = nullptr;
    std::atomic<float>* synthType3 = nullptr;

    std::atomic<float>* oscillators1Level = nullptr;
    std::atomic<float>* oscillators2Level = nullptr;
    std::atomic<float>* oscillators3Level = nullptr;


    std::atomic<float>* oscillators2Offset = nullptr;
    std::atomic<float>* oscillators3Offset = nullptr;

    std::atomic<float>* cutOff = nullptr;
    std::atomic<float>* resA = nullptr;
    std::atomic<float>* resB = nullptr;

    std::atomic<float>* gainDb = nullptr;
    std::atomic<float>* filtOn = nullptr;
    std::atomic<float>* combLfoOn = nullptr;
    std::atomic<float>* gainLfoOn = nullptr;
    std::atomic<float>* combLfoFreq = nullptr;
    std::atomic<float>* gainLfoFreq = nullptr;

    std::atomic<float>* filtType = nullptr;


    std::atomic<float>* gainMod = nullptr;
    std::atomic<float>* combMod = nullptr;

    std::atomic<float>* attack = nullptr;
    std::atomic<float>* decay = nullptr;
    std::atomic<float>* sustain = nullptr;
    std::atomic<float>* release = nullptr;


    //Dave Params
    std::atomic<float>* bitDepth = nullptr;
    std::atomic<float>* bitSpeed = nullptr;
    std::atomic<float>* delayTime = nullptr;
    std::atomic<float>* delayFeedback = nullptr;
    std::atomic<float>* delayMix = nullptr;

    std::atomic<float>* dampParameter = nullptr;
    std::atomic<float>* dryParameter = nullptr;
    std::atomic<float>* roomSizeParameter = nullptr;
    std::atomic<float>* wetParameter = nullptr;
    std::atomic<float>* widthParameter = nullptr;

    //DAVE FX
    BitcrusherData bitCrusher;
    DelayData delay;
    GetAGripverb verb;

    //juce::MidiKeyboardState &midiKeyBoardState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthzAudioProcessor)
};
