/*
  ==============================================================================

  Musical Ring Mod by Luke Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <array>

//==============================================================================
/**
*/
class MusicalRingModAudioProcessorEditor : public AudioProcessorEditor,
                                           private Timer,
                                           private MidiKeyboardStateListener
{
public:
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;


    MusicalRingModAudioProcessorEditor(MusicalRingModAudioProcessor&, AudioProcessorValueTreeState&,
                                       MidiKeyboardState&);
    ~MusicalRingModAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AudioProcessorValueTreeState& valueTreeState;
    MusicalRingModAudioProcessor& processor;

	DrawablePath bgPath;
	DrawablePath borderPath;
	Label nameLabel;

	GroupComponent freqGroup;
    Slider lfoFreqSlider;
    std::unique_ptr<SliderAttachment> lfoFreqSliderAttachment;

    Label lfoFreqSliderLabel;

	GroupComponent offsetsGroup;

    Slider offsetOctaveSlider;
    std::unique_ptr<SliderAttachment> offsetOctaveSliderAttachment;

    Slider offsetSemitoneSlider;
    std::unique_ptr<SliderAttachment> offsetSemitoneSliderAttachment;

    Slider offsetCentsSlider;
    std::unique_ptr<SliderAttachment> offsetCentsSliderAttachment;

    Slider standardSlider;
    std::unique_ptr<SliderAttachment> standardSliderAttachment;

	GroupComponent fOutGroup;

    Label fLabel;
    Label fcLabel;

	static constexpr int nFLabels = 6;
    std::array<Label, nFLabels> freqLabels;

    Label fValueLabel;
    Label fcValueLabel;

    std::array<Label, nFLabels> freqValueLabels;

    enum RadioButtonIds
    {
        frequencySourceButtons = 1001
    };

    ToggleButton midiSourceButton;
    std::unique_ptr<ButtonAttachment> freqToggleAttachment;

    ToggleButton sliderSourceButton;

    Slider depthSlider;
    std::unique_ptr<SliderAttachment> depthSliderAttachment;
	GroupComponent depthGroup;

    const String noteNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    MidiKeyboardComponent keyboard;
    MidiKeyboardState& keyboardState;

    // Inherited via MidiKeyboardStateListener
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    String frequencyToNoteName(double f) const;
	void setupFLabels();
	void setupDepthSlider();
    void setupOffsets();
    void setupSourceToggles();
    void setupLfoFreqSlider();
	void layoutFLabels(Rectangle<int>& fLabelsArea);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicalRingModAudioProcessorEditor)
};
