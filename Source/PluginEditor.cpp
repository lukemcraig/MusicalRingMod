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

    setSize (600, 300);

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

	fOutLabel_.setText("Output Frequencies:", dontSendNotification);
	addAndMakeVisible(fOutLabel_);

	f0Label_.setText("f0:", dontSendNotification);
	addAndMakeVisible(f0Label_);

	f1Label_.setText("f1:", dontSendNotification);
	addAndMakeVisible(f1Label_);

	f2Label_.setText("f2:", dontSendNotification);
	addAndMakeVisible(f2Label_);

	f3Label_.setText("f3:", dontSendNotification);
	addAndMakeVisible(f3Label_);

	f4Label_.setText("f4:", dontSendNotification);
	addAndMakeVisible(f4Label_);


	f0ValueLabel_.setText("", dontSendNotification);
	addAndMakeVisible(f0ValueLabel_);

	f1ValueLabel_.setText("", dontSendNotification);
	addAndMakeVisible(f1ValueLabel_);

	f2ValueLabel_.setText("", dontSendNotification);
	addAndMakeVisible(f2ValueLabel_);

	f3ValueLabel_.setText("", dontSendNotification);
	addAndMakeVisible(f3ValueLabel_);

	f4ValueLabel_.setText("", dontSendNotification);
	addAndMakeVisible(f4ValueLabel_);

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

	auto paneAreaWidth = area.getWidth() / 3;
	auto paneMargin = 5;

	auto freqArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
	auto fArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
	auto depthArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);

	lfoFreqSliderLabel_.setBounds(freqArea.removeFromTop(40).reduced(0, 10));
	midiSourceButton_.setBounds(freqArea.removeFromTop(40).reduced(20, 10));
	sliderSourceButton_.setBounds(freqArea.removeFromTop(40).reduced(20, 10));
	lfoFreqSlider_.setBounds(freqArea.reduced(20, 10));

	fOutLabel_.setBounds(fArea.removeFromTop(40).reduced(0, 10));
	auto fLeftArea = fArea.removeFromLeft(fArea.getWidth()/2);
	f0Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f1Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f2Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f3Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f4Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));

	f0ValueLabel_.setBounds(fArea.removeFromTop(40).reduced(0, 10));
	f1ValueLabel_.setBounds(fArea.removeFromTop(40).reduced(0, 10));
	f2ValueLabel_.setBounds(fArea.removeFromTop(40).reduced(0, 10));
	f3ValueLabel_.setBounds(fArea.removeFromTop(40).reduced(0, 10));
	f4ValueLabel_.setBounds(fArea.removeFromTop(40).reduced(0, 10));

	depthSliderLabel_.setBounds(depthArea.removeFromTop(40).reduced(0, 10));
	depthSlider_.setBounds(depthArea.reduced(20, 10));
}

void MusicalRingModAudioProcessorEditor::timerCallback()
{	
	if(*valueTreeState.getRawParameterValue(processor.PID_TOGGLE) == 1.0f)
		lfoFreqSlider_.setValue(processor.midiNumber_);
	// assuming the midi input is the input signal's fundamental frequency
	auto f = processor.midiNumber_;
	auto fc = lfoFreqSlider_.getValue();
	f0ValueLabel_.setText( String(f - fc) , dontSendNotification);
	f1ValueLabel_.setText(String(f + fc), dontSendNotification);
	f2ValueLabel_.setText(String(2*f - fc), dontSendNotification);
	f3ValueLabel_.setText(String(2 * f + fc), dontSendNotification);
	f4ValueLabel_.setText(String(3 * f - fc), dontSendNotification);
}
