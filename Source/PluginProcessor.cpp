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
         std::make_unique<juce::AudioParameterInt>("synthType","SynthType",0,3,0),
         std::make_unique<juce::AudioParameterInt>("cutOff","CutOff",10.f,100.f,10.f),
         std::make_unique<juce::AudioParameterFloat>("resA","ResA",-1.f,1.f,0.5f),
         std::make_unique<juce::AudioParameterFloat>("resB","ResB",0.01f,1.f,1.0f),
         std::make_unique<juce::AudioParameterFloat>("gainDb","Gain",-24.f,3.f,0.f),
         std::make_unique<juce::AudioParameterBool>("combLfoOn","CombLfo",true),
         std::make_unique<juce::AudioParameterBool>("gainLfoOn","GainLfo",true),
         std::make_unique<juce::AudioParameterInt>("lfoFreq","LfoFreq",1,20,2),
         std::make_unique<juce::AudioParameterChoice>("combType","CombType",juce::StringArray("Notch I","Notch II","Peak I","Peak II"),2),
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

    cutOff = parameters.getRawParameterValue("cutOff");
    resA = parameters.getRawParameterValue("resA");
    resB = parameters.getRawParameterValue("resB");
    gainDb = parameters.getRawParameterValue("gainDb");

    combLfoOn = parameters.getRawParameterValue("combLfoOn");
    gainLfoOn = parameters.getRawParameterValue("gainLfoOn");
    lfoFreq = parameters.getRawParameterValue("lfoFreq");

    combType = parameters.getRawParameterValue("combType");

    combMod = parameters.getRawParameterValue("combMod");
    gainMod = parameters.getRawParameterValue("gainMod");

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
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
        tempBuffer.clear(i, 0, tempBuffer.getNumSamples());
    }
        

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    int samplePos = 0;
    juce::MidiMessage currentMessage;
    juce::MidiBuffer::Iterator it(midiMessages);

    while (it.getNextEvent(currentMessage, samplePos))
    {
        if (currentMessage.isNoteOn())
        {
            //DBG("\n\n\n" + currentMessage.getDescription() + "\n" + juce::String(currentMessage.getNoteNumber()) + "\n\n\n");
            //oscillator.noteOn(currentMessage.getMidiNoteInHertz(currentMessage.getNoteNumber()));
            oscillators.push_back((new Oscillator(getSampleRate(), getNumOutputChannels(),buffer.getNumSamples(), currentMessage.getMidiNoteInHertz(currentMessage.getNoteNumber()),currentMessage.getNoteNumber())));
            //oscillators.push_back(*(new Oscillator(getSampleRate(), getNumOutputChannels(), buffer.getNumSamples(), currentMessage.getNoteNumber())));

        }
        if (currentMessage.isNoteOff())
        {
            //DBG("\n\n\n" + currentMessage.getDescription() + "\n" + juce::String(currentMessage.getNoteNumber()) + "\n\n\n");
            //oscillator.noteOff();
            for (int i = 0; i <oscillators.size(); i++)
            {
                //if(oscillators[it].)
                DBG(juce::String(oscillators[i]->noteNumber) + " zozododo " + juce::String(currentMessage.getNoteNumber()));
                //if (oscillators[i].fOsc == currentMessage.getMidiNoteInHertz(currentMessage.getNoteNumber()))
                if (oscillators[i]->noteNumber == currentMessage.getNoteNumber())
                {
                    //delete oscillators[i];

                    DBG("\n\nopopopopopopoopopopopopopopopopopo\n\n");
                    delete oscillators[i];
                    oscillators.erase(oscillators.begin()+i);
                }
            }
        }
    }
    //juce::AudioBuffer<float> dozo,bozo;
    //dozo= juce::AudioBuffer<float>(buffer.getNumChannels(), buffer.getNumChannels());
    //bozo = juce::AudioBuffer<float>(buffer.getNumChannels(), buffer.getNumChannels());
    //dozo.clear();
    //bozo.clear();
    
    //dozo.addFrom(0,0,dozo.getReadPointer(0), 0)
    //buffer.copyFrom(0, 0, dozo, 480);
    //buffer.makeCopyOf(dozo, true);
    for (int i = 0; i < oscillators.size(); i++)
    {
        tempBuffer=oscillators[i]->processBlock(buffer);
        for (int j = 0;j < tempBuffer.getNumChannels(); j++)
        {
            buffer.addFrom(j, 0, tempBuffer.getReadPointer(j),tempBuffer.getNumSamples(), 0.5 / oscillators.size());
        }
    }
    /*for (int j = 0; j < buffer.getNumChannels(); j++)
    {
        buffer.copyFrom(j, 0, bozo.getReadPointer(j),bozo.getNumSamples())
    }*/
    analyzer->pushSamples(buffer);
    levelMeter->pushSamples(buffer);
   // oscillator.processBlock(buffer);

    //FX Processing
    combFilter.processBlock(buffer, *cutOff, *resB, *resA, *combType, *lfoFreq, *combLfoOn, *combMod);
    gainMultiplier.processBlock(buffer, *gainDb, *lfoFreq, *gainLfoOn, *gainMod);
}

//==============================================================================
//bool SynthzAudioProcessor::hasEditor() const
//{
//    return true; // (change this to false if you choose to not supply an editor)
//}

//juce::AudioProcessorEditor* SynthzAudioProcessor::createEditor()
//{
//    return new SynthzAudioProcessorEditor (*this);
//    //return new juce::GenericAudioProcessorEditor(*this);
//}

//==============================================================================
//void SynthzAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
//{
//    // You should use this method to store your parameters in the memory block.
//    // You could do that either as raw data, or use the XML or ValueTree classes
//    // as intermediaries to make it easy to save and load complex data.
//}
//
//void SynthzAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
//{
//    // You should use this method to restore your parameters from this memory block,
//    // whose contents will have been created by the getStateInformation() call.
//}

//==============================================================================
// This creates new instances of the plugin..
//juce::ValueTree SynthzAudioProcessor::createGuiValueTree()
//{
//   //
//   // return 
//}


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthzAudioProcessor();
}
