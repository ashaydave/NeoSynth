/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
//#include "PluginEditor.h"

//==============================================================================
SynthzAudioProcessor::SynthzAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : foleys::MagicProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, juce::Identifier("Ripple"), {
         std::make_unique<juce::AudioParameterChoice>("synthType1","SynthType1",juce::StringArray("Sine","Sawtooth","Triangle","Square"),1),
         std::make_unique<juce::AudioParameterChoice>("synthType2","SynthType2",juce::StringArray("Sine","Sawtooth","Triangle","Square"),2),
         std::make_unique<juce::AudioParameterChoice>("synthType3","SynthType3",juce::StringArray("Sine","Sawtooth","Triangle","Square"),4),
         std::make_unique<juce::AudioParameterFloat>("oscillators1Level","Oscillators1Level",-24.f,3.f,0.f),
         std::make_unique<juce::AudioParameterFloat>("oscillators2Level","Oscillators2Level",-96.f,3.f,0.f),
         std::make_unique<juce::AudioParameterFloat>("oscillators3Level","Oscillators3Level",-96.f,3.f,0.f),
         std::make_unique<juce::AudioParameterInt>("cutOff","CutOff",0,20,2),
         std::make_unique<juce::AudioParameterFloat>("resA","ResA",-1.f,1.f,0.5f),
         std::make_unique<juce::AudioParameterFloat>("resB","ResB",0.01f,1.f,1.0f),
         std::make_unique<juce::AudioParameterFloat>("gainDb","Gain",-24.f,3.f,0.f),
         std::make_unique<juce::AudioParameterBool>("filtOn","FiltOn",true),
         std::make_unique<juce::AudioParameterBool>("combLfoOn","CombLfoOn",false),
         std::make_unique<juce::AudioParameterBool>("gainLfoOn","GainLfoOn",false),
         std::make_unique<juce::AudioParameterInt>("combLfoFreq","CombLfoFreq",0,20,0),
         std::make_unique<juce::AudioParameterInt>("gainLfoFreq","gainLfoFreq",1,20,2),
         std::make_unique<juce::AudioParameterChoice>("filtType","filtType",juce::StringArray("Notch I","Notch II","Peak I","Peak II","Moog"),1),
         std::make_unique<juce::AudioParameterFloat>("combMod","CombMod",0.0f,10.0f,2.0f),
         std::make_unique<juce::AudioParameterFloat>("gainMod","GainMod",0.0f,10.0f,3.0f),
         std::make_unique<juce::AudioParameterFloat>("attack","Attack",0.01f,1.f,0.1f),
         std::make_unique<juce::AudioParameterFloat>("release","Release",0.01f,1.f,0.1f),
         std::make_unique<juce::AudioParameterFloat>("sustain","Sustain",0.01f,1.f,1.0f),
         std::make_unique<juce::AudioParameterFloat>("decay","Decay",0.01f,1.f,0.1f),
         std::make_unique<juce::AudioParameterFloat>("bitDepth","BitDepth",2,16,16),
         std::make_unique<juce::AudioParameterFloat>("bitSpeed","BitSpeed",0.0f,1.f,1.0f),
         std::make_unique<juce::AudioParameterFloat>("delayTime","DelayTime",0.0f,2.0f,0.0f),
         std::make_unique<juce::AudioParameterFloat>("delayFeedback","DelayFeedback",0.0f,1.f,0.0f),
         std::make_unique<juce::AudioParameterFloat>("delayMix","DelayMix",0.0f,1.f,0.0f),
         std::make_unique<juce::AudioParameterFloat>("dampParameter","dampParameter",0.f,1.f,0.f),
         std::make_unique<juce::AudioParameterFloat>("dryParameter","dryParameter",0.f,1.f,1.f),
         std::make_unique<juce::AudioParameterFloat>("roomSizeParameter","roomSizeParameter",0.f,1.f,0.f),
         std::make_unique<juce::AudioParameterFloat>("wetParameter","wetParameter",0.f,1.f,0.f),
         std::make_unique<juce::AudioParameterFloat>("widthParameter","widthParameter",0.f,1.f,1.f),
         std::make_unique<juce::AudioParameterInt>("oscillators2Offset","oscillators2Offset",-12,12,0),
         std::make_unique<juce::AudioParameterInt>("oscillators3Offset","oscillators3Offset",-12,12,0)
                           })

#endif
{
    magicState.setGuiValueTree(BinaryData::Neo_xml, BinaryData::Neo_xmlSize);
    analyzer = magicState.createAndAddObject<foleys::MagicAnalyser>("input");
    levelMeter = magicState.createAndAddObject<foleys::MagicLevelSource>("levelInput");

    combFilter = RippleComb();
    gainMultiplier = RippleGain();

    synthType1 = parameters.getRawParameterValue("synthType1");
    synthType2 = parameters.getRawParameterValue("synthType2");
    synthType3 = parameters.getRawParameterValue("synthType3");

    oscillators1Level = parameters.getRawParameterValue("oscillators1Level");
    oscillators2Level = parameters.getRawParameterValue("oscillators2Level");
    oscillators3Level = parameters.getRawParameterValue("oscillators3Level");

    
    oscillators2Offset = parameters.getRawParameterValue("oscillators2Offset");
    oscillators3Offset = parameters.getRawParameterValue("oscillators3Offset");

    cutOff = parameters.getRawParameterValue("cutOff");
    resA = parameters.getRawParameterValue("resA");
    resB = parameters.getRawParameterValue("resB");
    gainDb = parameters.getRawParameterValue("gainDb");

    combLfoOn = parameters.getRawParameterValue("combLfoOn");
    gainLfoOn = parameters.getRawParameterValue("gainLfoOn");
    combLfoFreq = parameters.getRawParameterValue("combLfoFreq");
    gainLfoFreq = parameters.getRawParameterValue("gainLfoFreq");

    filtType = parameters.getRawParameterValue("filtType");

    filtOn = parameters.getRawParameterValue("filtOn");
    combMod = parameters.getRawParameterValue("combMod");
    gainMod = parameters.getRawParameterValue("gainMod");

    attack= parameters.getRawParameterValue("attack");
    decay = parameters.getRawParameterValue("decay");
    sustain = parameters.getRawParameterValue("release");
    release = parameters.getRawParameterValue("sustain");

    bitDepth = parameters.getRawParameterValue("bitDepth");
    bitSpeed = parameters.getRawParameterValue("bitSpeed");

    delayTime = parameters.getRawParameterValue("delayTime");
    delayFeedback = parameters.getRawParameterValue("delayFeedback");
    delayMix= parameters.getRawParameterValue("delayMix");

    dampParameter= parameters.getRawParameterValue("dampParameter");
    dryParameter = parameters.getRawParameterValue("dryParameter");
    roomSizeParameter = parameters.getRawParameterValue("roomSizeParameter");
    wetParameter = parameters.getRawParameterValue("wetParameter");
    widthParameter = parameters.getRawParameterValue("widthParameter");

    //magicState.addTrigger("filtOnButton", [&] { *filtOn= !*filtOn; });
    magicState.addTrigger("combLfoOnButton", [&] { *combLfoOn = !*combLfoOn; });
    magicState.addTrigger("gainLfoOnButton", [&] { *gainLfoOn = !*gainLfoOn; });



    
    juce::MidiKeyboardState& midiKeyBoardState = magicState.getKeyboardState();
    midiKeyBoardState.addListener(this);

    FOLEYS_SET_SOURCE_PATH(__FILE__);
}

SynthzAudioProcessor::~SynthzAudioProcessor()
{
}

//==============================================================================
const juce::String SynthzAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SynthzAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SynthzAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SynthzAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SynthzAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SynthzAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SynthzAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SynthzAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SynthzAudioProcessor::getProgramName (int index)
{
    return {};
}

void SynthzAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SynthzAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //oscillator.prepareToPlay(sampleRate,samplesPerBlock,getNumOutputChannels());
    analyzer->prepareToPlay(sampleRate, samplesPerBlock);
    levelMeter->setupSource(getTotalNumOutputChannels(), sampleRate, 500);
    tempBuffer1 = juce::AudioBuffer<float>(getNumOutputChannels(), samplesPerBlock);
    tempBuffer2 = juce::AudioBuffer<float>(getNumOutputChannels(), samplesPerBlock);
    tempBuffer3 = juce::AudioBuffer<float>(getNumOutputChannels(), samplesPerBlock);

    gainMultiplier.prepareToPlay(sampleRate);
    combFilter.prepareToPlay(sampleRate, samplesPerBlock, getNumOutputChannels());
    moogVA.prepareToPlay(sampleRate, samplesPerBlock, getNumInputChannels());

    delay.prepareToPlay(sampleRate, samplesPerBlock);
    verb.mute();
    adsr.setSampleRate(sampleRate);
}

void SynthzAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    verb.mute();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthzAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SynthzAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    samplesPerBlock = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
        tempBuffer1.clear(i, 0, tempBuffer1.getNumSamples());
        tempBuffer2.clear(i, 0, tempBuffer1.getNumSamples());
        tempBuffer3.clear(i, 0, tempBuffer1.getNumSamples());
    }
    
    


    int samplePos = 0;
    juce::MidiMessage currentMessage;
    juce::MidiBuffer::Iterator it(midiMessages);

    while (it.getNextEvent(currentMessage, samplePos))
    {
        if (currentMessage.isNoteOn())
            processMidiOn(currentMessage);
        else if (currentMessage.isNoteOff())
            processMidiOff(currentMessage);
    }

    for (int i = 0; i < oscillators1.size(); i++)
    {
        tempBuffer1=oscillators1[i]->processBlock(buffer,*synthType1,0);
        tempBuffer2 = oscillators2[i]->processBlock(buffer, *synthType2,(int)*oscillators2Offset);
        tempBuffer3 = oscillators3[i]->processBlock(buffer, *synthType3, (int)*oscillators3Offset);

        for (int j = 0;j < tempBuffer1.getNumChannels(); j++)
        {
            buffer.addFrom(j, 0, tempBuffer1.getReadPointer(j),tempBuffer1.getNumSamples(), juce::Decibels::decibelsToGain<float>(*oscillators1Level) / oscillators1.size());
            buffer.addFrom(j, 0, tempBuffer2.getReadPointer(j), tempBuffer1.getNumSamples(), juce::Decibels::decibelsToGain<float>(*oscillators2Level) / oscillators2.size());
            buffer.addFrom(j, 0, tempBuffer3.getReadPointer(j), tempBuffer1.getNumSamples(), juce::Decibels::decibelsToGain<float>(*oscillators3Level) / oscillators3.size());
        }
    }
    
    //ADSR
    adsrParams.attack = *attack;
    adsrParams.release = *release;
    adsrParams.sustain = *sustain;
    adsrParams.decay = *decay;
    adsr.setParameters(adsrParams);
    adsr.applyEnvelopeToBuffer(buffer, 0, buffer.getNumSamples());

   //juce::MidiMessage::getMidiNoteInHertz()
    //DBG(*filtType);
    
    //FX Processing
    DBG(*cutOff);

    if (*cutOff>0 && *filtType <= 3)
        combFilter.processBlock(buffer, *cutOff, *resB, *resA, *filtType, *combLfoFreq, *combLfoOn, *combMod);
    else if (*cutOff>0 && *filtType == 4)
        moogVA.processBlock(buffer, *cutOff*200, *resB, 0.0f, false);
    
    if(*bitSpeed>0)
        bitCrusher.process(buffer, *bitDepth, *bitSpeed);
    if(*delayTime>0.0f)
        delay.process(buffer, *delayTime, *delayMix, *delayFeedback);

    verb.setDamp(*dampParameter);
    verb.setDry(*dryParameter);
    verb.setRoomSize(*roomSizeParameter);
    verb.setWet(*wetParameter);
    verb.setWidth(*widthParameter);

    float* input_output_L = buffer.getWritePointer(0);
    float* input_output_R = buffer.getWritePointer(1);
    if(*wetParameter>0.0f)
        verb.processStereo(input_output_L, input_output_R, input_output_L, input_output_R, buffer.getNumSamples(), 1);

    gainMultiplier.processBlock(buffer, *gainDb, *gainLfoFreq, false, *gainMod);
   

    //Analysis
    analyzer->pushSamples(buffer);
    levelMeter->pushSamples(buffer);
}

//=============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthzAudioProcessor();
}

void SynthzAudioProcessor::handleNoteOn(juce::MidiKeyboardState* keyState, int midiChannel, int midiNoteNumber, float velocity)
{

    auto message = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    //message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    processMidiOn(message);
}

void SynthzAudioProcessor::handleNoteOff(juce::MidiKeyboardState* keyState, int midiChannel, int midiNoteNumber, float /*velocity*/)
{
    auto message = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    //message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    processMidiOff(message);
}

void SynthzAudioProcessor::processMidiOn(juce::MidiMessage message)
{
    oscillators1.push_back((new Oscillator(getSampleRate(), getNumOutputChannels(), samplesPerBlock, message.getMidiNoteInHertz(message.getNoteNumber()), message.getNoteNumber())));
    oscillators2.push_back((new Oscillator(getSampleRate(), getNumOutputChannels(), samplesPerBlock, message.getMidiNoteInHertz(message.getNoteNumber()), message.getNoteNumber())));
    oscillators3.push_back((new Oscillator(getSampleRate(), getNumOutputChannels(), samplesPerBlock, message.getMidiNoteInHertz(message.getNoteNumber()), message.getNoteNumber())));
    adsr.noteOn();
}
void SynthzAudioProcessor::processMidiOff(juce::MidiMessage message)
{
    adsr.noteOff();
    for (int i = 0; i < oscillators1.size(); i++)
    {
        //if(oscillators[it].)
        //DBG(juce::String(oscillators[i].fOsc) + " zozododo " + juce::String(currentMessage.getMidiNoteInHertz(currentMessage.getNoteNumber())));
        if (oscillators1[i]->noteNumber == message.getNoteNumber())
        {
            delete oscillators1[i];
            oscillators1.erase(oscillators1.begin() + i);
        }
        if (oscillators2[i]->noteNumber == message.getNoteNumber())
        {
            delete oscillators2[i];
            oscillators2.erase(oscillators2.begin() + i);
        }
        if (oscillators3[i]->noteNumber == message.getNoteNumber())
        {
            delete oscillators3[i];
            oscillators3.erase(oscillators3.begin() + i);
        }

    }

}
void SynthzAudioProcessor::combToggle()
{
    filtOn = false;
}