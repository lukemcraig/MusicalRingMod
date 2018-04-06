/*
  ==============================================================================

  Musical Ring Mod by Luke Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MusicalRingModAudioProcessorEditor::MusicalRingModAudioProcessorEditor (MusicalRingModAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
	lfoFreqSlider_.setSliderStyle(Slider::LinearHorizontal);
	lfoFreqSlider_.setRange(0,10000,0);
	lfoFreqSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, lfoFreqSlider_.getTextBoxHeight());
	lfoFreqSlider_.setPopupDisplayEnabled(true, false, this);
	lfoFreqSlider_.setTextValueSuffix("Hz");

	addAndMakeVisible(&lfoFreqSlider_);

	startTimerHz(30);
}

MusicalRingModAudioProcessorEditor::~MusicalRingModAudioProcessorEditor()
{
}

//==============================================================================
void MusicalRingModAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);    
}

void MusicalRingModAudioProcessorEditor::resized()
{
	const int LEFT_BOUND = 30;
	lfoFreqSlider_.setBounds(LEFT_BOUND, 30, 300, 40);
}

void MusicalRingModAudioProcessorEditor::timerCallback()
{
	lfoFreqSlider_.setValue(processor.midiNumber_);
}
