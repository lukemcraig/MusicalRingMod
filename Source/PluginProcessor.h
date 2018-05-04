/*
  ==============================================================================

  Musical Ring Mod by Luke Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class MusicalRingModAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    MusicalRingModAudioProcessor();
    ~MusicalRingModAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

	float convertMIDIToHz(float noteNumber, float semiToneOffset, float a4);

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================
	const String PID_LFO_FREQ = "lfofreq";
	const String PID_OFFSET_OCTAVES = "octaves";
	const String PID_OFFSET_SEMITONES = "semitones";
	const String PID_OFFSET_CENTS = "cents";
	const String PID_DEPTH = "depth";
	const String PID_TOGGLE_MIDI_SOURCE = "toggle";
	const String PID_STANDARD = "standard";	
	
	//==============================================================================
	float midiFreqOffsetted_;
	float midiFreq_;
	int midiNote_;
private:
	AudioProcessorValueTreeState parameters;

	float sampleRate_;
	float lfoInstantPhase_;

	float* parameterLfofreq_	= nullptr;
	float* parameterOctave_		= nullptr;
	float* parameterSemitone_	= nullptr;
	float* parameterCents_		= nullptr;
	float* parameterDepth_		= nullptr;
	float* parameterSource_		= nullptr;
	float* parameterStandard_	= nullptr;



    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicalRingModAudioProcessor)
		
};
