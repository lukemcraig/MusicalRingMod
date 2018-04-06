/*
  ==============================================================================
	
	Musical Ring Mod by Luke Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MusicalRingModAudioProcessor::MusicalRingModAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
	parameters(*this, nullptr)
{
	lfoInstantPhase_ = 0.0f;
	lfoFreq_ = 20.0f;

	parameters.createAndAddParameter(PID_LFO_FREQ, // parameter ID
		"LFO Frequency", // paramter Name
		String("Hz"), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 100.0f, 0, 0.5f), //range
		20.0f, // default value
		nullptr,
		nullptr);

}

MusicalRingModAudioProcessor::~MusicalRingModAudioProcessor()
{
}

//==============================================================================
const String MusicalRingModAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MusicalRingModAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MusicalRingModAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MusicalRingModAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MusicalRingModAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MusicalRingModAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MusicalRingModAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MusicalRingModAudioProcessor::setCurrentProgram (int index)
{
}

const String MusicalRingModAudioProcessor::getProgramName (int index)
{
    return {};
}

void MusicalRingModAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MusicalRingModAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	sampleRate_ = sampleRate;
}

void MusicalRingModAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MusicalRingModAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void MusicalRingModAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	const int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	auto* channelData = buffer.getWritePointer(0);
	for (int sample = 0; sample < numSamples; ++sample) {
		float in = channelData[sample];

		float depth = 1.0f;
		// m[n] = 1 - a + a * cos(n * wc)
		float carrier = 1.0f - depth + depth * cos(2.0f * float_Pi * lfoInstantPhase_);
		// y[n]= m[n] * x[n]
		float out = carrier * in;
		channelData[sample] = out;

		//update the other channels with the same sample value
		for (int channel = 1; channel < totalNumInputChannels; ++channel)
		{
			buffer.getWritePointer(channel)[sample] = out;
		}

		lfoInstantPhase_ += lfoFreq_ * (1.0f / sampleRate_);
	}
}

//==============================================================================
bool MusicalRingModAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MusicalRingModAudioProcessor::createEditor()
{
    return new MusicalRingModAudioProcessorEditor (*this);
}

//==============================================================================
void MusicalRingModAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MusicalRingModAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MusicalRingModAudioProcessor();
}
