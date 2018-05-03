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

    setSize (400, 300);

	lfoFreqSliderLabel_.setText("Frequency:",dontSendNotification);
	addAndMakeVisible(lfoFreqSliderLabel_);

	lfoFreqSlider_.setSliderStyle(Slider::LinearVertical);
	lfoFreqSlider_.setRange(0,10000,0.0);
	lfoFreqSlider_.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider_.getTextBoxWidth(), lfoFreqSlider_.getTextBoxHeight());
	addAndMakeVisible(&lfoFreqSlider_);
	lfoFreqSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_LFO_FREQ, lfoFreqSlider_));
	
	midiSourceButton_.setButtonText("Midi");
	addAndMakeVisible(midiSourceButton_);
	freqToggleAttachment_.reset(new ButtonAttachment(valueTreeState, processor.PID_TOGGLE, midiSourceButton_));
	midiSourceButton_.setRadioGroupId(FrequencySourceButtons);
	
	sliderSourceButton_.setButtonText("Slider");
	addAndMakeVisible(sliderSourceButton_);	
	sliderSourceButton_.setRadioGroupId(FrequencySourceButtons);
	sliderSourceButton_.setToggleState(!midiSourceButton_.getToggleState(),false);

	depthSliderLabel_.setText("Depth:", dontSendNotification);
	addAndMakeVisible(depthSliderLabel_);

	depthSlider_.setSliderStyle(Slider::LinearVertical);
	depthSlider_.setRange(0, 1.0, 0.00);
	depthSlider_.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider_.getTextBoxWidth(), depthSlider_.getTextBoxHeight());
	addAndMakeVisible(&depthSlider_);
	depthSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_DEPTH, depthSlider_));

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
	auto area = getLocalBounds();
	// margins
	area.reduce(10, 10);	

	auto paneAreaWidth = area.getWidth() / 2;
	auto paneMargin = 5;

	auto freqArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
	auto depthArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);

	lfoFreqSliderLabel_.setBounds(freqArea.removeFromTop(40).reduced(0, 10));
	midiSourceButton_.setBounds(freqArea.removeFromTop(40).reduced(20, 10));
	sliderSourceButton_.setBounds(freqArea.removeFromTop(40).reduced(20, 10));
	lfoFreqSlider_.setBounds(freqArea.reduced(20, 10));
	

	depthSliderLabel_.setBounds(depthArea.removeFromTop(40).reduced(0, 10));
	depthSlider_.setBounds(depthArea.reduced(20, 10));
}

void MusicalRingModAudioProcessorEditor::timerCallback()
{	
	if(*valueTreeState.getRawParameterValue(processor.PID_TOGGLE) == 1.0f)
		lfoFreqSlider_.setValue(processor.midiNumber_);
}
