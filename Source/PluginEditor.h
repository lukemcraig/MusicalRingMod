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
	public AudioProcessorValueTreeState::Listener,
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
	void parameterChanged(const String &parameterID, float newValue) override;
private:
	
	AudioProcessorValueTreeState& valueTreeState;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MusicalRingModAudioProcessor& processor;

	Slider lfoFreqSlider_;
	std::unique_ptr<SliderAttachment> lfoFreqSliderAttachment_;

	ToggleButton freqToggle_;
	std::unique_ptr<ButtonAttachment> freqToggleAttachment_;

	bool updateSlider_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MusicalRingModAudioProcessorEditor)
};
