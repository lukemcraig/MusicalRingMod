/*
  ==============================================================================

  Musical Ring Mod by Luke Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MusicalRingModAudioProcessorEditor::MusicalRingModAudioProcessorEditor (MusicalRingModAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts)
{
	updateSlider_ = true;

    setSize (400, 300);
	lfoFreqSlider_.setSliderStyle(Slider::LinearHorizontal);
	lfoFreqSlider_.setRange(0,10000,0);
	lfoFreqSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 120, lfoFreqSlider_.getTextBoxHeight());
	lfoFreqSlider_.setPopupDisplayEnabled(true, false, this);
	lfoFreqSlider_.setTextValueSuffix("Hz");
	addAndMakeVisible(&lfoFreqSlider_);
	lfoFreqSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_LFO_FREQ, lfoFreqSlider_));
	//valueTreeState.addParameterListener(processor.PID_LFO_FREQ, this);

	freqToggle_.setButtonText("Midi 0 or Slider 1");
	addAndMakeVisible(freqToggle_);
	freqToggleAttachment_.reset(new ButtonAttachment(valueTreeState, processor.PID_TOGGLE, freqToggle_));
	valueTreeState.addParameterListener(processor.PID_TOGGLE, this);

	startTimerHz(30);
}

MusicalRingModAudioProcessorEditor::~MusicalRingModAudioProcessorEditor()
{
	valueTreeState.removeParameterListener(processor.PID_TOGGLE, this);
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
	freqToggle_.setBounds(LEFT_BOUND, lfoFreqSlider_.getBottom(), 300, 40);
}

void MusicalRingModAudioProcessorEditor::timerCallback()
{	
	if(updateSlider_)
		lfoFreqSlider_.setValue(processor.midiNumber_);
}

void MusicalRingModAudioProcessorEditor::parameterChanged(const String & parameterID, float newValue)
{
	if (parameterID == processor.PID_TOGGLE) {
		updateSlider_ = newValue==0;
	}
}
