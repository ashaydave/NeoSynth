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
         std::make_unique<juce::AudioParameterChoice>("synthType","SynthType",juce::StringArray("Sine","Sawtooth","Triangle","Square"),1),
         std::make_unique<juce::AudioParameterInt>("cutOff","CutOff",10.f,100.f,10.f),
         std::make_unique<juce::AudioParameterFloat>("resA","ResA",-1.f,1.f,0.5f),
         std::make_unique<juce::AudioParameterFloat>("resB","ResB",0.01f,1.f,1.0f),
         std::make_unique<juce::AudioParameterFloat>("gainDb","Gain",-24.f,3.f,0.f),
         std::make_unique<juce::AudioParameterBool>("combOn","CombOn",true),
         std::make_unique<juce::AudioParameterBool>("combLfoOn","CombLfoOn",true),
         std::make_unique<juce::AudioParameterBool>("gainLfoOn","GainLfoOn",true),
         std::make_unique<juce::AudioParameterInt>("combLfoFreq","CombLfoFreq",1,20,2),
         std::make_unique<juce::AudioParameterInt>("gainLfoFreq","gainLfoFreq",1,20,2),
         std::make_unique<juce::AudioParameterChoice>("combType","CombType",juce::StringArray("Notch I","Notch II","Peak I","Peak II"),1),
         std::make_unique<juce::AudioParameterFloat>("combMod","CombMod",0.0f,10.0f,2.0f),
         std::make_unique<juce::AudioParameterFloat>("gainMod","GainMod",0.0f,10.0f,3.0f)
                           })

#endif
{
    magicState.setGuiValueTree(BinaryData::Synthz_xml, BinaryData::Synthz_xmlSize);
    analyzer = magicState.createAndAddObject<foleys::MagicAnalyser>("input");
    levelMeter = magicState.createAndAddObject<foleys::MagicLevelSource>("levelInput");

    combFilter = RippleComb();
    gainMultiplier = RippleGain();

    synthType = parameters.getRawParameterValue("synthType");


    cutOff = parameters.getRawParameterValue("cutOff");
    resA = parameters.getRawParameterValue("resA");
    resB = parameters.getRawParameterValue("resB");
    gainDb = parameters.getRawParameterValue("gainDb");

    combLfoOn = parameters.getRawParameterValue("combLfoOn");
    gainLfoOn = parameters.getRawParameterValue("gainLfoOn");
    combLfoFreq = parameters.getRawParameterValue("combLfoFreq");
    gainLfoFreq = parameters.getRawParameterValue("gainLfoFreq");

    combType = parameters.getRawParameterValue("combType");

    combOn = parameters.getRawParameterValue("combOn");
    combMod = parameters.getRawParameterValue("combMod");
    gainMod = parameters.getRawParameterValue("gainMod");

    magicState.addTrigger("combOnButton", [&] { *combOn = !*combOn; });
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
    tempBuffer = juce::AudioBuffer<float>(getNumOutputChannels(), samplesPerBlock);

    gainMultiplier.prepareToPlay(sampleRate);
    combFilter.prepareToPlay(sampleRate, samplesPerBlock, getNumOutputChannels());
}

void SynthzAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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
        tempBuffer.clear(i, 0, tempBuffer.getNumSamples());
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

    for (int i = 0; i < oscillators.size(); i++)
    {
        tempBuffer=oscillators[i]->processBlock(buffer,*synthType);
        for (int j = 0;j < tempBuffer.getNumChannels(); j++)
        {
            buffer.addFrom(j, 0, tempBuffer.getReadPointer(j),tempBuffer.getNumSamples(), 0.5 / oscillators.size());
        }
    }
    

   

    //FX Processing
    if(*combOn)
        combFilter.processBlock(buffer, *cutOff, *resB, *resA, *combType, *combLfoFreq, *combLfoOn, *combMod);

    gainMultiplier.processBlock(buffer, *gainDb, *gainLfoFreq, *gainLfoOn, *gainMod);
   

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
    oscillators.push_back((new Oscillator(getSampleRate(), getNumOutputChannels(), samplesPerBlock, message.getMidiNoteInHertz(message.getNoteNumber()), message.getNoteNumber())));
}
void SynthzAudioProcessor::processMidiOff(juce::MidiMessage message)
{
    for (int i = 0; i < oscillators.size(); i++)
    {
        //if(oscillators[it].)
        //DBG(juce::String(oscillators[i].fOsc) + " zozododo " + juce::String(currentMessage.getMidiNoteInHertz(currentMessage.getNoteNumber())));
        if (oscillators[i]->noteNumber == message.getNoteNumber())
        {
            delete oscillators[i];
            oscillators.erase(oscillators.begin() + i);
        }
    }
}
void SynthzAudioProcessor::combToggle()
{
    combOn = false;
}