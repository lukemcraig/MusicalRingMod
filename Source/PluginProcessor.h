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
const String pidLfoFreq = "lfoFreq";
const String pidOffsetOctaves = "octaves";
const String pidOffsetSemitones = "semitones";
const String pidOffsetCents = "cents";
const String pidDepth = "depth";
const String pidToggleMidiSource = "toggle";
const String pidStandard = "standard";
//==============================================================================
class MusicalRingModAudioProcessor : public AudioProcessor,
                                     private MidiKeyboardStateListener
{
public:
    //==============================================================================
    MusicalRingModAudioProcessor();
    ~MusicalRingModAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

    float convertMidiToHz(float noteNumber, float semiToneOffset, float a4) const;

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
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================

    float midiFreqAndOffset;
    float midiFreq;
    int midiNote;
private:
    AudioProcessorValueTreeState parameters;
    MidiKeyboardState keyboardState;

    float sampleRate;
    float lfoInstantPhase;

    float* parameterLfoFreq = nullptr;
    float* parameterOctave = nullptr;
    float* parameterSemitone = nullptr;
    float* parameterCents = nullptr;
    float* parameterDepth = nullptr;
    float* parameterSource = nullptr;
    float* parameterStandard = nullptr;

    float previousDepth;

    static float linearInterpolate(float y0, float y1, float t);

    // Inherited via MidiKeyboardStateListener
    virtual void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    virtual void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicalRingModAudioProcessor)
};
