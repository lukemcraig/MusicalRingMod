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
    void setupFLabels();
    ~MusicalRingModAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
    void layoutFLabels(Rectangle<int>& fLabelsArea);
    void timerCallback() override;

private:
    AudioProcessorValueTreeState& valueTreeState;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MusicalRingModAudioProcessor& processor;

    Slider lfoFreqSlider;
    std::unique_ptr<SliderAttachment> lfoFreqSliderAttachment;
    Label lfoFreqSliderLabel;

    Label offsetsLabel;

    Slider offsetOctaveSlider;
    std::unique_ptr<SliderAttachment> offsetOctaveSliderAttachment;

    Slider offsetSemitoneSlider;
    std::unique_ptr<SliderAttachment> offsetSemitoneSliderAttachment;

    Slider offsetCentsSlider;
    std::unique_ptr<SliderAttachment> offsetCentsSliderAttachment;

    Slider standardSlider;
    std::unique_ptr<SliderAttachment> standardSliderAttachment;

    Label fOutLabel;

    Label fLabel;
    Label fcLabel;

    std::array<Label, 6> freqLabels;

    Label fValueLabel;
    Label fcValueLabel;

    std::array<Label, 6> freqValueLabels;

    enum RadioButtonIds
    {
        frequencySourceButtons = 1001
    };

    ToggleButton midiSourceButton;
    std::unique_ptr<ButtonAttachment> freqToggleAttachment;
    ToggleButton sliderSourceButton;

    Slider depthSlider;
    std::unique_ptr<SliderAttachment> depthSliderAttachment;
    Label depthSliderLabel;

    String frequencyToNoteName(double f) const;

    const std::string noteNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    MidiKeyboardComponent keyboard;
    MidiKeyboardState& keyboardState;

    // Inherited via MidiKeyboardStateListener
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicalRingModAudioProcessorEditor)
};
