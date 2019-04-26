/*
  ==============================================================================

  Musical Ring Mod by Luke Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MusicalRingModAudioProcessorEditor::MusicalRingModAudioProcessorEditor(MusicalRingModAudioProcessor& p,
                                                                       AudioProcessorValueTreeState& vts,
                                                                       MidiKeyboardState& ks)
    : AudioProcessorEditor(&p), processor(p), valueTreeState(vts), keyboardState(ks),
      keyboard(ks, MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(800, 600);

    lfoFreqSliderLabel.setText("Frequency:", dontSendNotification);
    addAndMakeVisible(lfoFreqSliderLabel);

    lfoFreqSlider.setSliderStyle(Slider::LinearVertical);

    lfoFreqSlider.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider.getTextBoxWidth(),
                                  lfoFreqSlider.getTextBoxHeight());
    addAndMakeVisible(&lfoFreqSlider);
    lfoFreqSliderAttachment.reset(new SliderAttachment(valueTreeState, processor.pidLfoFreq, lfoFreqSlider));

    midiSourceButton.setButtonText("Midi");
    addAndMakeVisible(midiSourceButton);
    freqToggleAttachment.reset(new ButtonAttachment(valueTreeState, processor.pidToggleMidiSource,
                                                    midiSourceButton));
    midiSourceButton.setRadioGroupId(frequencySourceButtons);

    sliderSourceButton.setButtonText("Slider");
    addAndMakeVisible(sliderSourceButton);
    sliderSourceButton.setRadioGroupId(frequencySourceButtons);
    sliderSourceButton.setToggleState(!midiSourceButton.getToggleState(), false);

    // --------

    offsetsLabel.setText("Offsets:", dontSendNotification);
    addAndMakeVisible(offsetsLabel);

    offsetOctaveSlider.setSliderStyle(Slider::IncDecButtons);
    offsetOctaveSlider.setTextBoxStyle(Slider::TextBoxAbove, false, offsetOctaveSlider.getTextBoxWidth(),
                                       offsetOctaveSlider.getTextBoxHeight());
    addAndMakeVisible(&offsetOctaveSlider);
    offsetOctaveSliderAttachment.reset(
        new SliderAttachment(valueTreeState, processor.pidOffsetOctaves, offsetOctaveSlider));

    offsetSemitoneSlider.setSliderStyle(Slider::IncDecButtons);
    offsetSemitoneSlider.setTextBoxStyle(Slider::TextBoxAbove, false, offsetSemitoneSlider.getTextBoxWidth(),
                                         offsetSemitoneSlider.getTextBoxHeight());
    addAndMakeVisible(&offsetSemitoneSlider);
    offsetSemitoneSliderAttachment.reset(
        new SliderAttachment(valueTreeState, processor.pidOffsetSemitones, offsetSemitoneSlider));

    offsetCentsSlider.setSliderStyle(Slider::IncDecButtons);
    offsetCentsSlider.setTextBoxStyle(Slider::TextBoxAbove, false, offsetCentsSlider.getTextBoxWidth(),
                                      offsetCentsSlider.getTextBoxHeight());
    addAndMakeVisible(&offsetCentsSlider);
    offsetCentsSliderAttachment.reset(
        new SliderAttachment(valueTreeState, processor.pidOffsetCents, offsetCentsSlider));

    standardSlider.setSliderStyle(Slider::IncDecButtons);
    standardSlider.setTextBoxStyle(Slider::TextBoxAbove, false, standardSlider.getTextBoxWidth(),
                                   standardSlider.getTextBoxHeight());
    addAndMakeVisible(&standardSlider);
    standardSliderAttachment.reset(new SliderAttachment(valueTreeState, processor.pidStandard, standardSlider));

    // --------

    depthSliderLabel.setText("Depth:", dontSendNotification);
    addAndMakeVisible(depthSliderLabel);

    depthSlider.setSliderStyle(Slider::LinearVertical);

    depthSlider.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider.getTextBoxWidth(),
                                depthSlider.getTextBoxHeight());
    addAndMakeVisible(&depthSlider);
    depthSliderAttachment.reset(new SliderAttachment(valueTreeState, processor.pidDepth, depthSlider));

    setupFLabels();

    addAndMakeVisible(keyboard);
    keyboardState.addListener(this);

    startTimerHz(30);
}

void MusicalRingModAudioProcessorEditor::setupFLabels()
{
    fOutLabel.setText("Output Frequencies:", dontSendNotification);
    addAndMakeVisible(fOutLabel);

    fLabel.setText("f:", dontSendNotification);
    addAndMakeVisible(fLabel);

    fcLabel.setText("fc:", dontSendNotification);
    addAndMakeVisible(fcLabel);

    f0Label.setText("f0:", dontSendNotification);
    addAndMakeVisible(f0Label);

    f1Label.setText("f1:", dontSendNotification);
    addAndMakeVisible(f1Label);

    f2Label.setText("f2:", dontSendNotification);
    addAndMakeVisible(f2Label);

    f3Label.setText("f3:", dontSendNotification);
    addAndMakeVisible(f3Label);

    f4Label.setText("f4:", dontSendNotification);
    addAndMakeVisible(f4Label);

    f5Label.setText("f5:", dontSendNotification);
    addAndMakeVisible(f5Label);

    fValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(fValueLabel);

    fcValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(fcValueLabel);

    f0ValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(f0ValueLabel);

    f1ValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(f1ValueLabel);

    f2ValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(f2ValueLabel);

    f3ValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(f3ValueLabel);

    f4ValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(f4ValueLabel);

    f5ValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(f5ValueLabel);
}

MusicalRingModAudioProcessorEditor::~MusicalRingModAudioProcessorEditor()
{
    keyboardState.removeListener(this);
}


//==============================================================================
void MusicalRingModAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid color)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::white);
    g.setFont(15.0f);
}

void MusicalRingModAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    // margins
    area.reduce(10, 10);

    const auto paneAreaWidth = area.getWidth() / 4;
    const auto paneMargin = 5;

    const auto keyboardArea = area.removeFromBottom(100);

    auto freqArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
    auto offsetsArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
    auto fLabelsArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);
    auto depthArea = area.removeFromLeft(paneAreaWidth).reduced(paneMargin);

    lfoFreqSliderLabel.setBounds(freqArea.removeFromTop(40).reduced(0, 10));
    midiSourceButton.setBounds(freqArea.removeFromTop(40).reduced(20, 10));
    sliderSourceButton.setBounds(freqArea.removeFromTop(40).reduced(20, 10));
    lfoFreqSlider.setBounds(freqArea.reduced(20, 10));

    offsetsLabel.setBounds(offsetsArea.removeFromTop(40).reduced(0, 10));
    offsetOctaveSlider.setBounds(offsetsArea.removeFromTop(80).reduced(20, 10));
    offsetSemitoneSlider.setBounds(offsetsArea.removeFromTop(80).reduced(20, 10));
    offsetCentsSlider.setBounds(offsetsArea.removeFromTop(80).reduced(20, 10));
    standardSlider.setBounds(offsetsArea.removeFromTop(80).reduced(20, 10));

    layoutFLabels(fLabelsArea);

    depthSliderLabel.setBounds(depthArea.removeFromTop(40).reduced(0, 10));
    depthSlider.setBounds(depthArea.reduced(20, 10));

    keyboard.setBounds(keyboardArea);
}

void MusicalRingModAudioProcessorEditor::layoutFLabels(juce::Rectangle<int>& fLabelsArea)
{
    fOutLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    auto fLeftArea = fLabelsArea.removeFromLeft(fLabelsArea.getWidth() / 2);

    fLabel.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
    fcLabel.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));

    f0Label.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
    f1Label.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
    f2Label.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
    f3Label.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
    f4Label.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
    f5Label.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));

    fValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    fcValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));

    f0ValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    f1ValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    f2ValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    f3ValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    f4ValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    f5ValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
}

void MusicalRingModAudioProcessorEditor::timerCallback()
{
    if (*valueTreeState.getRawParameterValue(processor.pidToggleMidiSource) == 1.0f)
    {
        lfoFreqSlider.setValue(processor.midiFreqAndOffset);
        offsetSemitoneSlider.setVisible(true);
        offsetCentsSlider.setVisible(true);
        offsetOctaveSlider.setVisible(true);
        standardSlider.setVisible(true);
    }
    else
    {
        offsetSemitoneSlider.setVisible(false);
        offsetCentsSlider.setVisible(false);
        offsetOctaveSlider.setVisible(false);
        standardSlider.setVisible(false);
    }
    // assuming the midi input is the input signal's fundamental frequency
    const auto f = processor.midiFreq;
    const auto fc = lfoFreqSlider.getValue();

    fValueLabel.setText(String(f) + "Hz,  " + frequencyToNoteName(f), dontSendNotification);
    fcValueLabel.setText(String(fc) + "Hz,  " + frequencyToNoteName(fc), dontSendNotification);
    const auto f0 = f - fc;
    f0ValueLabel.setText(String(f0) + "Hz,  " + frequencyToNoteName(f0), dontSendNotification);
    const auto f1 = f + fc;
    f1ValueLabel.setText(String(f1) + "Hz,  " + frequencyToNoteName(f1), dontSendNotification);
    const auto f2 = 2 * f - fc;
    f2ValueLabel.setText(String(f2) + "Hz,  " + frequencyToNoteName(f2), dontSendNotification);
    const auto f3 = 2 * f + fc;
    f3ValueLabel.setText(String(f3) + "Hz,  " + frequencyToNoteName(f3), dontSendNotification);
    const auto f4 = 3 * f - fc;
    f4ValueLabel.setText(String(f4) + "Hz,  " + frequencyToNoteName(f4), dontSendNotification);
    const auto f5 = 3 * f + fc;
    f5ValueLabel.setText(String(f5) + "Hz,  " + frequencyToNoteName(f5), dontSendNotification);
}

String MusicalRingModAudioProcessorEditor::frequencyToNoteName(const double f) const
{
    const auto halfStepsAboveC0 = roundDoubleToInt(12.0 * log2(f / 16.3516));
    if (halfStepsAboveC0 <= 12)
        return String("");
    const auto octave = halfStepsAboveC0 / 12;
    const auto noteIndex = halfStepsAboveC0 % 12;
    return String(noteNames[noteIndex]) + String(octave);
}

void MusicalRingModAudioProcessorEditor::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                                      float velocity)
{
    DBG("hi");
}

void MusicalRingModAudioProcessorEditor::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                                       float velocity)
{
}
