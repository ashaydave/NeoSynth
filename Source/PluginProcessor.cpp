/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthzAudioProcessor::SynthzAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
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

void SynthzAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    samplesPerBlock = buffer.getNumSamples();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

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
            oscillators.push_back(*(new Oscillator(getSampleRate(), getNumOutputChannels(),buffer.getNumSamples(), currentMessage.getMidiNoteInHertz(currentMessage.getNoteNumber()),currentMessage.getNoteNumber())));
            //oscillators.push_back(*(new Oscillator(getSampleRate(), getNumOutputChannels(), buffer.getNumSamples(), currentMessage.getNoteNumber())));

        }
        if (currentMessage.isNoteOff())
        {
            //DBG("\n\n\n" + currentMessage.getDescription() + "\n" + juce::String(currentMessage.getNoteNumber()) + "\n\n\n");
            //oscillator.noteOff();
            for (int i = 0; i <oscillators.size(); i++)
            {
                //if(oscillators[it].)
                DBG(juce::String(oscillators[i].noteNumber) + " zozododo " + juce::String(currentMessage.getNoteNumber()));
                //if (oscillators[i].fOsc == currentMessage.getMidiNoteInHertz(currentMessage.getNoteNumber()))
                if (oscillators[i].noteNumber == currentMessage.getNoteNumber())
                {
                    //delete oscillators[i];

                    DBG("\n\nopopopopopopoopopopopopopopopopopo\n\n");
                    oscillators.erase(oscillators.begin()+i);
                }
            }
        }
    }

    for (int i = 0; i < oscillators.size(); i++)
        oscillators[i].processBlock(buffer);
    
   // oscillator.processBlock(buffer);
}

//==============================================================================
bool SynthzAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SynthzAudioProcessor::createEditor()
{
    return new SynthzAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SynthzAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SynthzAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthzAudioProcessor();
}
