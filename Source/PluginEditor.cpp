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

    setSize (800, 400);

	lfoFreqSliderLabel_.setText("Frequency:",dontSendNotification);
	addAndMakeVisible(lfoFreqSliderLabel_);

	lfoFreqSlider_.setSliderStyle(Slider::LinearVertical);

	lfoFreqSlider_.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider_.getTextBoxWidth(), lfoFreqSlider_.getTextBoxHeight());
	addAndMakeVisible(&lfoFreqSlider_);
	lfoFreqSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_LFO_FREQ, lfoFreqSlider_));
	
	midiSourceButton_.setButtonText("Midi");
	addAndMakeVisible(midiSourceButton_);
	freqToggleAttachment_.reset(new ButtonAttachment(valueTreeState, processor.PID_TOGGLE_MIDI_SOURCE, midiSourceButton_));
	midiSourceButton_.setRadioGroupId(FrequencySourceButtons);
	
	sliderSourceButton_.setButtonText("Slider");
	addAndMakeVisible(sliderSourceButton_);	
	sliderSourceButton_.setRadioGroupId(FrequencySourceButtons);
	sliderSourceButton_.setToggleState(!midiSourceButton_.getToggleState(),false);

	// --------

	offsetsLabel_.setText("Offsets:", dontSendNotification);
	addAndMakeVisible(offsetsLabel_);

	offsetOctaveSlider_.setSliderStyle(Slider::IncDecButtons);
	offsetOctaveSlider_.setTextBoxStyle(Slider::TextBoxAbove, false, offsetOctaveSlider_.getTextBoxWidth(), offsetOctaveSlider_.getTextBoxHeight());
	addAndMakeVisible(&offsetOctaveSlider_);
	offsetOctaveSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_OFFSET_OCTAVES, offsetOctaveSlider_));
	
	offsetSemitoneSlider_.setSliderStyle(Slider::IncDecButtons);	
	offsetSemitoneSlider_.setTextBoxStyle(Slider::TextBoxAbove, false, offsetSemitoneSlider_.getTextBoxWidth(), offsetSemitoneSlider_.getTextBoxHeight());
	addAndMakeVisible(&offsetSemitoneSlider_);
	offsetSemitoneSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_OFFSET_SEMITONES, offsetSemitoneSlider_));

	offsetCentsSlider_.setSliderStyle(Slider::IncDecButtons);
	offsetCentsSlider_.setTextBoxStyle(Slider::TextBoxAbove, false, offsetCentsSlider_.getTextBoxWidth(), offsetCentsSlider_.getTextBoxHeight());
	addAndMakeVisible(&offsetCentsSlider_);
	offsetCentsSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_OFFSET_CENTS, offsetCentsSlider_));

	standardSlider_.setSliderStyle(Slider::IncDecButtons);
	standardSlider_.setTextBoxStyle(Slider::TextBoxAbove, false, standardSlider_.getTextBoxWidth(), standardSlider_.getTextBoxHeight());
	addAndMakeVisible(&standardSlider_);
	standardSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_STANDARD, standardSlider_));

	// --------

	depthSliderLabel_.setText("Depth:", dontSendNotification);
	addAndMakeVisible(depthSliderLabel_);

	depthSlider_.setSliderStyle(Slider::LinearVertical);

	depthSlider_.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider_.getTextBoxWidth(), depthSlider_.getTextBoxHeight());
	addAndMakeVisible(&depthSlider_);
	depthSliderAttachment_.reset(new SliderAttachment(valueTreeState, processor.PID_DEPTH, depthSlider_));

	setupFLabels();

	startTimerHz(30);
}

void MusicalRingModAudioProcessorEditor::setupFLabels()
{
	fOutLabel_.setText("Output Frequencies:", dontSendNotification);
	addAndMakeVisible(fOutLabel_);

	fLabel_.setText("f:", dontSendNotification);
	addAndMakeVisible(fLabel_);

	fcLabel_.setText("fc:", dontSendNotification);
	addAndMakeVisible(fcLabel_);

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

	f5Label_.setText("f5:", dontSendNotification);
	addAndMakeVisible(f5Label_);

	fValueLabel_.setText("", dontSendNotification);
	addAndMakeVisible(fValueLabel_);

	fcValueLabel_.setText("", dontSendNotification);
	addAndMakeVisible(fcValueLabel_);

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

	f5ValueLabel_.setText("", dontSendNotification);
	addAndMakeVisible(f5ValueLabel_);
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

	auto paneAreaWidth = area.getWidth() / 4;
	auto paneMargin = 5;

	auto freqArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
	auto offsetsArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
	auto fLabelsArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
	auto depthArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);

	lfoFreqSliderLabel_.setBounds(freqArea.removeFromTop(40).reduced(0, 10));
	midiSourceButton_.setBounds(freqArea.removeFromTop(40).reduced(20, 10));
	sliderSourceButton_.setBounds(freqArea.removeFromTop(40).reduced(20, 10));
	lfoFreqSlider_.setBounds(freqArea.reduced(20, 10));

	offsetsLabel_.setBounds(offsetsArea.removeFromTop(40).reduced(0, 10));
	offsetOctaveSlider_.setBounds(offsetsArea.removeFromTop(80).reduced(20, 10));
	offsetSemitoneSlider_.setBounds(offsetsArea.removeFromTop(80).reduced(20, 10));
	offsetCentsSlider_.setBounds(offsetsArea.removeFromTop(80).reduced(20, 10));
	standardSlider_.setBounds(offsetsArea.removeFromTop(80).reduced(20, 10));

	layoutFLabels(fLabelsArea);

	depthSliderLabel_.setBounds(depthArea.removeFromTop(40).reduced(0, 10));
	depthSlider_.setBounds(depthArea.reduced(20, 10));
}

void MusicalRingModAudioProcessorEditor::layoutFLabels(juce::Rectangle<int> &fLabelsArea)
{
	fOutLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
	auto fLeftArea = fLabelsArea.removeFromLeft(fLabelsArea.getWidth() / 2);

	fLabel_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	fcLabel_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));

	f0Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f1Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f2Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f3Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f4Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
	f5Label_.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));

	fValueLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
	fcValueLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));

	f0ValueLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
	f1ValueLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
	f2ValueLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
	f3ValueLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
	f4ValueLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
	f5ValueLabel_.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
}

void MusicalRingModAudioProcessorEditor::timerCallback()
{	
	if (*valueTreeState.getRawParameterValue(processor.PID_TOGGLE_MIDI_SOURCE) == 1.0f) {
		lfoFreqSlider_.setValue(processor.midiFreqOffsetted_);
		offsetSemitoneSlider_.setVisible(true);
		offsetCentsSlider_.setVisible(true);
		offsetOctaveSlider_.setVisible(true);
		standardSlider_.setVisible(true);
	}
	else {
		offsetSemitoneSlider_.setVisible(false);
		offsetCentsSlider_.setVisible(false);
		offsetOctaveSlider_.setVisible(false);
		standardSlider_.setVisible(false);
	}
	// assuming the midi input is the input signal's fundamental frequency
	auto f = processor.midiFreq_;
	auto fc = lfoFreqSlider_.getValue();

	fValueLabel_.setText(String(f)		+ "Hz,  " + frequencyToNoteName(f), dontSendNotification);
	fcValueLabel_.setText(String(fc)	+ "Hz,  " + frequencyToNoteName(fc), dontSendNotification);
	auto f0 = f - fc;						    
	f0ValueLabel_.setText(String(f0)	+ "Hz,  " + frequencyToNoteName(f0), dontSendNotification);
	auto f1 = f + fc;						    
	f1ValueLabel_.setText(String(f1)	+ "Hz,  "+ frequencyToNoteName(f1), dontSendNotification);
	auto f2 = 2 * f - fc;					    
	f2ValueLabel_.setText(String(f2)	+ "Hz,  " + frequencyToNoteName(f2), dontSendNotification);
	auto f3 = 2 * f + fc;					    
	f3ValueLabel_.setText(String(f3)	+ "Hz,  " + frequencyToNoteName(f3), dontSendNotification);
	auto f4 = 3 * f - fc;					    
	f4ValueLabel_.setText(String(f4)	+ "Hz,  " + frequencyToNoteName(f4), dontSendNotification);
	auto f5 = 3 * f + fc;					    
	f5ValueLabel_.setText(String(f5)	+ "Hz,  " + frequencyToNoteName(f5), dontSendNotification);
}

String MusicalRingModAudioProcessorEditor::frequencyToNoteName(double f) {
	auto halfStepsAboveC0 = roundDoubleToInt(12.0 * log2(f/16.3516));
	if (halfStepsAboveC0 <= 12)
		return String("");
	auto octave = halfStepsAboveC0 / 12;
	auto noteIndex = halfStepsAboveC0 % 12;
	return String(noteNames[noteIndex]) + String(octave);
}
