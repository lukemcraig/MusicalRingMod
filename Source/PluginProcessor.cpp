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
	midiFreqOffsetted_ = 0.0f;

	parameters.createAndAddParameter(PID_LFO_FREQ, // parameter ID
		"LFO Frequency", // paramter Name
		String("Hz"), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 10000.0f,0,0.5f), //range
		20.0f, // default value
		[](float value)
		{
			// value to text function (C++11 lambda)
			return String(value,3) ;
		},
		[](const String& text)
		{
			// text to value function (C++11 lambda)			
			return text.getFloatValue();
		});

	parameters.createAndAddParameter(PID_OFFSET, // parameter ID
		"Offset", // paramter Name
		String("semitones"), // parameter label (suffix)
		NormalisableRange<float>(-64, 64, 1), //range
		0.0f, // default value
		nullptr,
		nullptr
		);

	parameters.createAndAddParameter(PID_DEPTH, // parameter ID
		"Modulation Depth", // paramter Name
		String("%"), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 1.0f, 0), //range
		1.0f, // default value
		[](float value)
		{
			// value to text function (C++11 lambda)
			return String(value*100.0f,2)+String("%");
		},
		[](const String& text)
		{
			// text to value function (C++11 lambda)			
			return text.getFloatValue()*0.01;
		});
	parameters.createAndAddParameter(PID_TOGGLE, // parameter ID
		"Freq Source", // paramter Name
		String(""), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 1.0f, 0), //range
		0.0f, // default value
		[](float value)
		{
			// value to text function (C++11 lambda)
			return value < 0.5 ? "Midi" : "Slider";
		},
		[](const String& text)
		{
			// text to value function (C++11 lambda)
			if (text == "Midi")    return 0.0f;
			if (text == "Slider")  return 1.0f;
			return 0.0f;
		});

	parameters.state = ValueTree(Identifier("RingModParameters"));
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

	MidiMessage mResult;
	int mSamplePosition;
	for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(mResult, mSamplePosition);)
	{
		if (mResult.isNoteOn())
		{
			// convert the midi number to Hz, assuming A is 440Hz
			midiFreq_ = convertMIDIToHz(mResult.getNoteNumber(), 0, 440);
			midiFreqOffsetted_ = convertMIDIToHz(mResult.getNoteNumber(), *parameters.getRawParameterValue(PID_OFFSET), 440);	

		}
	}

	auto* channelData = buffer.getWritePointer(0);
	for (int sample = 0; sample < numSamples; ++sample) {
		float in = channelData[sample];

		float depth = *parameters.getRawParameterValue(PID_DEPTH);
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
		float lfoFreq = midiFreqOffsetted_ ;
		if (*parameters.getRawParameterValue(PID_TOGGLE) == 0.0f) {
			lfoFreq = *parameters.getRawParameterValue(PID_LFO_FREQ);
		}
		//DBG(lfoFreq);
		lfoInstantPhase_ += lfoFreq * (1.0f / sampleRate_);
		// wrap the instantaneous phase from 0.0 to 1.0
		if (lfoInstantPhase_ >= 1.0f) {
			lfoInstantPhase_ -= 1.0f;
		}
		jassert(lfoInstantPhase_<1.0f);
	}
}

float MusicalRingModAudioProcessor::convertMIDIToHz(float noteNumber, float semiToneOffset, float a4)
{
	return a4 * pow(2.0f, (noteNumber + semiToneOffset - 69.0f) / 12.0f);
}

//==============================================================================
bool MusicalRingModAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MusicalRingModAudioProcessor::createEditor()
{
    return new MusicalRingModAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void MusicalRingModAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // store parameters
	ScopedPointer<XmlElement> xml(parameters.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void MusicalRingModAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    //restore parameters
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.state = ValueTree::fromXml(*xmlState);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MusicalRingModAudioProcessor();
}
