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
	parameters_(*this, nullptr)
{	
	lfoInstantPhase_ = 0.0f;
	midiFreqOffsetted_ = 0.0f;	
	midiNote_ = 0;
	midiFreq_ = 0;
	midiFreqOffsetted_ = 0;

	parameters_.createAndAddParameter(PID_LFO_FREQ, // parameter ID
		"LFO Frequency", // paramter Name
		String(""), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 10000.0f,0,0.5f), //range
		20.0f, // default value
		[](float value)
		{
			// value to text function (C++11 lambda)
			return String(value,3) + String("Hz");
		},
		nullptr
		);

	parameters_.createAndAddParameter(PID_OFFSET_OCTAVES, // parameter ID
		"Octaves", // paramter Name
		String(""), // parameter label (suffix)
		NormalisableRange<float>(-10, 10, 1), //range
		0, // default value
		[](float value)
		{
		// value to text function (C++11 lambda)
		return String(value, 0) + String(" octaves");
		},
		nullptr
		);

	parameters_.createAndAddParameter(PID_OFFSET_SEMITONES, // parameter ID
		"Semitones", // paramter Name
		String(""), // parameter label (suffix)
		NormalisableRange<float>(-100, 100, 1), //range
		0, // default value
		[](float value)
		{
		// value to text function (C++11 lambda)
		return String(value, 0) + String(" semitones");
		},
		nullptr
		);

	parameters_.createAndAddParameter(PID_OFFSET_CENTS, // parameter ID
		"Cents", // paramter Name
		String(""), // parameter label (suffix)
		NormalisableRange<float>(-100, 100, 1), //range
		0, // default value
		[](float value)
		{
		// value to text function (C++11 lambda)
		return String(value, 0) + String(" cents");
		},
		nullptr
		);

	parameters_.createAndAddParameter(PID_DEPTH, // parameter ID
		"Modulation Depth", // paramter Name
		String(""), // parameter label (suffix)
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
	parameters_.createAndAddParameter(PID_TOGGLE_MIDI_SOURCE, // parameter ID
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

	parameters_.createAndAddParameter(PID_STANDARD, // parameter ID
		"Pitch Standard", // paramter Name
		String(""), // parameter label (suffix)
		NormalisableRange<float>(300.0f, 500.0f, 0.1f), //range
		440.0f, // default value
		[](float value)
		{
		// value to text function (C++11 lambda)
		return String(value, 1) + String("Hz");
		},
		nullptr
		);

	parameters_.state = ValueTree(Identifier("RingModParameters"));

	parameterLfofreq_	= parameters_.getRawParameterValue(PID_LFO_FREQ);
	parameterOctave_	= parameters_.getRawParameterValue(PID_OFFSET_OCTAVES);
	parameterSemitone_	= parameters_.getRawParameterValue(PID_OFFSET_SEMITONES);
	parameterCents_		= parameters_.getRawParameterValue(PID_OFFSET_CENTS);
	parameterDepth_		= parameters_.getRawParameterValue(PID_DEPTH);
	parameterSource_	= parameters_.getRawParameterValue(PID_TOGGLE_MIDI_SOURCE);
	parameterStandard_	= parameters_.getRawParameterValue(PID_STANDARD);

	keyboardState_.addListener(this);
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
			// convert the midi number to Hz			
			midiNote_ = mResult.getNoteNumber();
			
		}
	}

	if (midiNote_ != 0) {
		midiFreq_ = convertMIDIToHz(midiNote_, 0, *parameterStandard_);
		auto semitoneOffset = (*parameterOctave_ * 12) + *parameterSemitone_ + (*parameterCents_*.01);
		midiFreqOffsetted_ = convertMIDIToHz(midiNote_, semitoneOffset, *parameterStandard_);
	}
	else {
		midiFreq_ = 0;
		midiFreqOffsetted_ = 0;
	}

	auto* channelData = buffer.getWritePointer(0);
	for (int sample = 0; sample < numSamples; ++sample) {
		float in = channelData[sample];

		float depth = *parameters_.getRawParameterValue(PID_DEPTH);
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
		if (*parameterSource_== 0.0f) {
			lfoFreq = *parameterLfofreq_;
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
    return new MusicalRingModAudioProcessorEditor (*this, parameters_, keyboardState_);
}

//==============================================================================
void MusicalRingModAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // store parameters
	ScopedPointer<XmlElement> xml(parameters_.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void MusicalRingModAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    //restore parameters
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState != nullptr)
		if (xmlState->hasTagName(parameters_.state.getType()))
			parameters_.state = ValueTree::fromXml(*xmlState);
}

void MusicalRingModAudioProcessor::handleNoteOn(MidiKeyboardState * source, int midiChannel, int midiNoteNumber, float velocity)
{
	DBG("Yo");
	midiNote_ = midiNoteNumber;
}

void MusicalRingModAudioProcessor::handleNoteOff(MidiKeyboardState * source, int midiChannel, int midiNoteNumber, float velocity)
{
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MusicalRingModAudioProcessor();
}
