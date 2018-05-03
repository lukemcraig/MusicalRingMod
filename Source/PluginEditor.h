/*
  ==============================================================================

  Musical Ring Mod by Luke Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MusicalRingModAudioProcessorEditor  : public AudioProcessorEditor,
	private Timer
{
public:
	typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    MusicalRingModAudioProcessorEditor (MusicalRingModAudioProcessor&, AudioProcessorValueTreeState&);
    ~MusicalRingModAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void timerCallback() override;
	

private:
	
	AudioProcessorValueTreeState& valueTreeState;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MusicalRingModAudioProcessor& processor;

	Slider lfoFreqSlider_;
	std::unique_ptr<SliderAttachment> lfoFreqSliderAttachment_;
	Label lfoFreqSliderLabel_;

	Label fOutLabel_;

	Label fLabel_;
	Label fcLabel_;

	Label f0Label_;
	Label f1Label_;
	Label f2Label_;
	Label f3Label_;
	Label f4Label_;
	Label f5Label_;

	Label fValueLabel_;
	Label fcValueLabel_;

	Label f0ValueLabel_;
	Label f1ValueLabel_;
	Label f2ValueLabel_;
	Label f3ValueLabel_;
	Label f4ValueLabel_;
	Label f5ValueLabel_;

	enum RadioButtonIds
	{
		FrequencySourceButtons = 1001
	};

	ToggleButton midiSourceButton_;
	std::unique_ptr<ButtonAttachment> freqToggleAttachment_;
	ToggleButton sliderSourceButton_;

	Slider depthSlider_;
	std::unique_ptr<SliderAttachment> depthSliderAttachment_;
	Label depthSliderLabel_;

	String frequencyToNoteName(double f);

	const std::string noteNames[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MusicalRingModAudioProcessorEditor)
};
