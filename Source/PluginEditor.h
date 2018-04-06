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
    MusicalRingModAudioProcessorEditor (MusicalRingModAudioProcessor&);
    ~MusicalRingModAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MusicalRingModAudioProcessor& processor;

	Slider lfoFreqSlider_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MusicalRingModAudioProcessorEditor)
};
