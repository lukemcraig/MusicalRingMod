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
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                     .withInput("Input #1", AudioChannelSet::stereo(), true)
                     .withInput("Input #2", AudioChannelSet::stereo(), false)
#endif
          .withOutput("Output #1", AudioChannelSet::stereo(), true)
          .withOutput("Output #2", AudioChannelSet::stereo(), false)
#endif
      ),
#endif
      parameters(*this, nullptr)
{
    lfoInstantPhase = 0.0f;
    midiFreqAndOffset = 0.0f;
    midiNote = 0;
    midiFreq = 0;
    midiFreqAndOffset = 0;
	sampleRate = 0;
	previousDepth = 0;

    parameters.createAndAddParameter(pidLfoFreq, // parameter ID
                                     "LFO Frequency", // parameter Name
                                     String(""), // parameter label (suffix)
                                     NormalisableRange<float>(0.0f, 10000.0f, 0, 0.5f), //range
                                     20.0f, // default value
                                     [](float value)
                                     {
                                         // value to text function (C++11 lambda)
                                         return String(value, 3) + String("Hz");
                                     },
                                     nullptr
    );

    parameters.createAndAddParameter(pidOffsetOctaves, // parameter ID
                                     "Octaves", // parameter Name
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

    parameters.createAndAddParameter(pidOffsetSemitones, // parameter ID
                                     "Semitones", // parameter Name
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

    parameters.createAndAddParameter(pidOffsetCents, // parameter ID
                                     "Cents", // parameter Name
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

    parameters.createAndAddParameter(pidDepth, // parameter ID
                                     "Modulation Depth", // parameter Name
                                     String(""), // parameter label (suffix)
                                     NormalisableRange<float>(0.0f, 1.0f, 0), //range
                                     1.0f, // default value
                                     [](float value)
                                     {
                                         // value to text function (C++11 lambda)
                                         return String(value * 100.0f, 2) + String("%");
                                     },
                                     [](const String& text)
                                     {
                                         // text to value function (C++11 lambda)			
                                         return text.getFloatValue() * 0.01;
                                     });
    parameters.createAndAddParameter(pidToggleMidiSource, // parameter ID
                                     "Freq Source", // parameter Name
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
                                         if (text == "Midi") return 0.0f;
                                         if (text == "Slider") return 1.0f;
                                         return 0.0f;
                                     });

    parameters.createAndAddParameter(pidStandard, // parameter ID
                                     "Pitch Standard", // parameter Name
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

    parameters.state = ValueTree(Identifier("RingModParameters"));

    parameterLfoFreq = parameters.getRawParameterValue(pidLfoFreq);
    parameterOctave = parameters.getRawParameterValue(pidOffsetOctaves);
    parameterSemitone = parameters.getRawParameterValue(pidOffsetSemitones);
    parameterCents = parameters.getRawParameterValue(pidOffsetCents);
    parameterDepth = parameters.getRawParameterValue(pidDepth);
    parameterSource = parameters.getRawParameterValue(pidToggleMidiSource);
    parameterStandard = parameters.getRawParameterValue(pidStandard);

    keyboardState.addListener(this);
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
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int MusicalRingModAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MusicalRingModAudioProcessor::setCurrentProgram(int index)
{
}

const String MusicalRingModAudioProcessor::getProgramName(int index)
{
    return {};
}

void MusicalRingModAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void MusicalRingModAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialization that you need..
    this->sampleRate = sampleRate;
    previousDepth = *parameterDepth;
}

void MusicalRingModAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MusicalRingModAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void MusicalRingModAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const auto nextDepth = *parameterDepth;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        MidiMessage mResult;
        int mSamplePosition;
        for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(mResult, mSamplePosition);)
        {
            if (mSamplePosition < sample)
                continue;
            if (mSamplePosition > sample)
                break;

            if (mResult.isNoteOn())
            {
                midiNote = mResult.getNoteNumber();
            }
        }
        if (midiNote != 0)
        {
            midiFreq = convertMidiToHz(midiNote, 0, *parameterStandard);
            const auto semitoneOffset = (*parameterOctave * 12) + *parameterSemitone + (*parameterCents * .01);
            midiFreqAndOffset = convertMidiToHz(midiNote, semitoneOffset, *parameterStandard);
        }
        else
        {
            midiFreq = 0;
            midiFreqAndOffset = 0;
        }

        // linear interpolation to avoid clicks and pops on depth change
        const auto depth = linearInterpolate(previousDepth, nextDepth, float(sample + 1) / float(numSamples));
        // m[n] = 1 - a + a * cos(n * wc)
        const auto carrier = 1.0f - depth + depth * cos(2.0f * float_Pi * lfoInstantPhase);

        //update all the channels
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            // y[n]= m[n] * x[n]		

            channelData[sample] = carrier * channelData[sample];
        }

        auto lfoFreq = midiFreqAndOffset;
        if (*parameterSource == 0.0f)
        {
            lfoFreq = *parameterLfoFreq;
        }
        //DBG(lfoFreq);
        lfoInstantPhase += lfoFreq * (1.0f / sampleRate);
        // wrap the instantaneous phase from 0.0 to 1.0
        if (lfoInstantPhase >= 1.0f)
        {
            lfoInstantPhase -= 1.0f;
        }
        jassert(lfoInstantPhase<1.0f);
    }
    previousDepth = nextDepth;
}

float MusicalRingModAudioProcessor::convertMidiToHz(const float noteNumber, const float semiToneOffset, const float a4) const
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
    return new MusicalRingModAudioProcessorEditor(*this, parameters, keyboardState);
}

//==============================================================================
void MusicalRingModAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // store parameters
    const ScopedPointer<XmlElement> xml(parameters.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MusicalRingModAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    //restore parameters
    const ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.state = ValueTree::fromXml(*xmlState);
}

float MusicalRingModAudioProcessor::linearInterpolate(const float y0, const float y1, const float t) const
{
    return y0 + t * (y1 - y0);
}

void MusicalRingModAudioProcessor::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                                float velocity)
{
    // TODO
    midiNote = midiNoteNumber;
}

void MusicalRingModAudioProcessor::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                                 float velocity)
{
    // TODO
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MusicalRingModAudioProcessor();
}
