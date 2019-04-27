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
    : AudioProcessorEditor(&p), valueTreeState(vts), processor(p),
      keyboard(ks, MidiKeyboardComponent::horizontalKeyboard), keyboardState(ks)
{
    setSize(800, 600);

    lfoFreqSliderLabel.setText("Frequency:", dontSendNotification);
    addAndMakeVisible(lfoFreqSliderLabel);

    lfoFreqSlider.setSliderStyle(Slider::LinearVertical);

    lfoFreqSlider.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider.getTextBoxWidth(),
                                  lfoFreqSlider.getTextBoxHeight());
    addAndMakeVisible(&lfoFreqSlider);
    lfoFreqSliderAttachment.reset(new SliderAttachment(valueTreeState, pidLfoFreq, lfoFreqSlider));

    midiSourceButton.setButtonText("Midi");
    addAndMakeVisible(midiSourceButton);
    freqToggleAttachment.reset(new ButtonAttachment(valueTreeState, pidToggleMidiSource,
                                                    midiSourceButton));
    midiSourceButton.setRadioGroupId(frequencySourceButtons);

    sliderSourceButton.setButtonText("Slider");
    addAndMakeVisible(sliderSourceButton);
    sliderSourceButton.setRadioGroupId(frequencySourceButtons);
    sliderSourceButton.setToggleState(!midiSourceButton.getToggleState(), dontSendNotification);

    // --------

    offsetsLabel.setText("Offsets:", dontSendNotification);
    addAndMakeVisible(offsetsLabel);

    offsetOctaveSlider.setSliderStyle(Slider::IncDecButtons);
    offsetOctaveSlider.setTextBoxStyle(Slider::TextBoxAbove, false, offsetOctaveSlider.getTextBoxWidth(),
                                       offsetOctaveSlider.getTextBoxHeight());
    addAndMakeVisible(&offsetOctaveSlider);
    offsetOctaveSliderAttachment.reset(
        new SliderAttachment(valueTreeState, pidOffsetOctaves, offsetOctaveSlider));

    offsetSemitoneSlider.setSliderStyle(Slider::IncDecButtons);
    offsetSemitoneSlider.setTextBoxStyle(Slider::TextBoxAbove, false, offsetSemitoneSlider.getTextBoxWidth(),
                                         offsetSemitoneSlider.getTextBoxHeight());
    addAndMakeVisible(&offsetSemitoneSlider);
    offsetSemitoneSliderAttachment.reset(
        new SliderAttachment(valueTreeState, pidOffsetSemitones, offsetSemitoneSlider));

    offsetCentsSlider.setSliderStyle(Slider::IncDecButtons);
    offsetCentsSlider.setTextBoxStyle(Slider::TextBoxAbove, false, offsetCentsSlider.getTextBoxWidth(),
                                      offsetCentsSlider.getTextBoxHeight());
    addAndMakeVisible(&offsetCentsSlider);
    offsetCentsSliderAttachment.reset(
        new SliderAttachment(valueTreeState, pidOffsetCents, offsetCentsSlider));

    standardSlider.setSliderStyle(Slider::IncDecButtons);
    standardSlider.setTextBoxStyle(Slider::TextBoxAbove, false, standardSlider.getTextBoxWidth(),
                                   standardSlider.getTextBoxHeight());
    addAndMakeVisible(&standardSlider);
    standardSliderAttachment.reset(new SliderAttachment(valueTreeState, pidStandard, standardSlider));

    // --------

    depthSliderLabel.setText("Depth:", dontSendNotification);
    addAndMakeVisible(depthSliderLabel);

    depthSlider.setSliderStyle(Slider::LinearVertical);

    depthSlider.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider.getTextBoxWidth(),
                                depthSlider.getTextBoxHeight());
    addAndMakeVisible(&depthSlider);
    depthSliderAttachment.reset(new SliderAttachment(valueTreeState, pidDepth, depthSlider));

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

    {
        auto i = 0;
        for (auto& freqLabel : freqLabels)
        {
            freqLabel.setText("f" + String(i++) + ":", dontSendNotification);
            addAndMakeVisible(freqLabel);
        }
    }

    fValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(fValueLabel);

    fcValueLabel.setText("", dontSendNotification);
    addAndMakeVisible(fcValueLabel);

    for (auto& freqValueLabel : freqValueLabels)
    {
        freqValueLabel.setText("", dontSendNotification);
        addAndMakeVisible(freqValueLabel);
    }
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
    g.drawFittedText("Musical Ring Mod - Luke M. Craig - " __DATE__ + String(" ") + __TIME__, getLocalBounds(), Justification::topLeft, 1);
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

void MusicalRingModAudioProcessorEditor::layoutFLabels(Rectangle<int>& fLabelsArea)
{
    fOutLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    auto fLeftArea = fLabelsArea.removeFromLeft(fLabelsArea.getWidth() / 2);

    fLabel.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
    fcLabel.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));

    for (auto& freqLabel : freqLabels)
    {
        freqLabel.setBounds(fLeftArea.removeFromTop(40).reduced(0, 10));
    }

    fValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    fcValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));

    for (auto& freqValueLabel : freqValueLabels)
    {
        freqValueLabel.setBounds(fLabelsArea.removeFromTop(40).reduced(0, 10));
    }
}

void MusicalRingModAudioProcessorEditor::timerCallback()
{
    if (*valueTreeState.getRawParameterValue(pidToggleMidiSource) == 1.0f)
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

    {
        auto i = 0;
        for (auto& freqValueLabel : freqValueLabels)
        {
            const int j = (i / 2) + 1;
            const auto evenOrOdd = i % 2 ? 1 : -1;
            const auto freqValue = j * f + (evenOrOdd * fc);
            freqValueLabel.setText(String(freqValue) + "Hz,  " + frequencyToNoteName(freqValue), dontSendNotification);
            ++i;
        }
    }
}

String MusicalRingModAudioProcessorEditor::frequencyToNoteName(const double f) const
{
    const auto halfStepsAboveC0 = roundToInt(12.0 * log2(f / 16.3516));
    if (halfStepsAboveC0 <= 12)
        return String("");
    const auto octave = halfStepsAboveC0 / 12;
    const auto noteIndex = halfStepsAboveC0 % 12;
    return String(noteNames[noteIndex]) + String(octave);
}

void MusicalRingModAudioProcessorEditor::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                                      float velocity)
{
    // TODO
    DBG("handleNoteOn()");
}

void MusicalRingModAudioProcessorEditor::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                                       float velocity)
{
}
