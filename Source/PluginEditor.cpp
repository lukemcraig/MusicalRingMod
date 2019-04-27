/*
  ==============================================================================

  Musical Ring Mod by Luke Craig

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

MusicalRingModAudioProcessorEditor::MusicalRingModAudioProcessorEditor(MusicalRingModAudioProcessor& p,
                                                                       AudioProcessorValueTreeState& vts,
                                                                       MidiKeyboardState& ks)
    : AudioProcessorEditor(&p), valueTreeState(vts), processor(p),
      keyboard(ks, MidiKeyboardComponent::horizontalKeyboard), keyboardState(ks)
{
    setSize(800, 600);

    setupLfoFreqSlider();

    setupSourceToggles();

    setupOffsets();

    setupDepthSlider();

    setupFLabels();

    addAndMakeVisible(keyboard);
    keyboardState.addListener(this);

    startTimerHz(30);
}

MusicalRingModAudioProcessorEditor::~MusicalRingModAudioProcessorEditor()
{
    keyboardState.removeListener(this);
	stopTimer();
}

void MusicalRingModAudioProcessorEditor::setupDepthSlider()
{
    depthSliderLabel.setText("Depth:", dontSendNotification);
    addAndMakeVisible(depthSliderLabel);

    depthSlider.setSliderStyle(Slider::LinearBar);

    depthSlider.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider.getTextBoxWidth(),
                                depthSlider.getTextBoxHeight());
    addAndMakeVisible(&depthSlider);
    depthSliderAttachment.reset(new SliderAttachment(valueTreeState, pidDepth, depthSlider));
}

void MusicalRingModAudioProcessorEditor::setupOffsets()
{
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
}

void MusicalRingModAudioProcessorEditor::setupSourceToggles()
{
    midiSourceButton.setButtonText("Midi");
    addAndMakeVisible(midiSourceButton);
    freqToggleAttachment.reset(new ButtonAttachment(valueTreeState, pidToggleMidiSource,
                                                    midiSourceButton));
    midiSourceButton.setRadioGroupId(frequencySourceButtons);

    sliderSourceButton.setButtonText("Slider");
    addAndMakeVisible(sliderSourceButton);
    sliderSourceButton.setRadioGroupId(frequencySourceButtons);
    sliderSourceButton.setToggleState(!midiSourceButton.getToggleState(), dontSendNotification);
}

void MusicalRingModAudioProcessorEditor::setupLfoFreqSlider()
{
    lfoFreqSliderLabel.setText("Frequency:", dontSendNotification);
    addAndMakeVisible(lfoFreqSliderLabel);

    lfoFreqSlider.setSliderStyle(Slider::LinearBar);

    lfoFreqSlider.setTextBoxStyle(Slider::TextBoxBelow, false, depthSlider.getTextBoxWidth(),
                                  lfoFreqSlider.getTextBoxHeight());
    addAndMakeVisible(&lfoFreqSlider);
    lfoFreqSliderAttachment.reset(new SliderAttachment(valueTreeState, pidLfoFreq, lfoFreqSlider));
}

void MusicalRingModAudioProcessorEditor::setupFLabels()
{
    fOutLabel.setText("Input/Output Frequencies:", dontSendNotification);
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


//==============================================================================
void MusicalRingModAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid color)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Musical Ring Mod - Luke M. Craig - " __DATE__ + String(" ") + __TIME__, getLocalBounds(),
                     Justification::topLeft, 1);
}

void MusicalRingModAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    // margins
    area.reduce(10, 10);

    auto nPanes = 2;
    const auto midiVisible = *valueTreeState.getRawParameterValue(pidToggleMidiSource);
    if (midiVisible == 1.0f)
        nPanes += 2;
    const auto paneAreaHeight = area.getHeight() / nPanes;
    const auto paneMargin = 5;

    {
        auto depthAndFLabelsArea = area.removeFromTop(paneAreaHeight).reduced(paneMargin);
        auto depthArea = depthAndFLabelsArea.removeFromLeft(depthAndFLabelsArea.proportionOfWidth(0.5f));
        depthSliderLabel.setBounds(depthArea.removeFromTop(40).reduced(0, 10));
        depthSlider.setBounds(depthArea.reduced(20, 10));
        layoutFLabels(depthAndFLabelsArea);
    }

    {
        auto freqArea = area.removeFromTop(paneAreaHeight).reduced(paneMargin);

        auto freqAreaTop = freqArea.removeFromTop(20);
        lfoFreqSliderLabel.setBounds(freqAreaTop.removeFromLeft(80));
        midiSourceButton.setBounds(freqAreaTop.removeFromLeft(80));
        sliderSourceButton.setBounds(freqAreaTop.removeFromLeft(80));

        lfoFreqSlider.setBounds(freqArea.reduced(20, 10));
    }

    if (midiVisible == 1.0f)
    {
        auto offsetsArea = area.removeFromTop(paneAreaHeight).reduced(paneMargin);

        offsetsLabel.setBounds(offsetsArea.removeFromTop(40).reduced(0, 10));
        const auto nSliders = 4;
        const auto sliderWidth = offsetsArea.proportionOfWidth(1.0f / nSliders);
        offsetOctaveSlider.setBounds(offsetsArea.removeFromLeft(sliderWidth).reduced(20, 10));
        offsetSemitoneSlider.setBounds(offsetsArea.removeFromLeft(sliderWidth).reduced(20, 10));
        offsetCentsSlider.setBounds(offsetsArea.removeFromLeft(sliderWidth).reduced(20, 10));
        standardSlider.setBounds(offsetsArea.removeFromLeft(sliderWidth).reduced(20, 10));
    }

    {
        const auto keyboardArea = area.removeFromTop(paneAreaHeight);
        keyboard.setBounds(keyboardArea);
    }
}

void MusicalRingModAudioProcessorEditor::layoutFLabels(Rectangle<int>& fLabelsArea)
{
    const auto space = 20;
    fOutLabel.setBounds(fLabelsArea.removeFromTop(20));
    {
        auto fLeftArea = fLabelsArea.removeFromLeft(40);

        fLabel.setBounds(fLeftArea.removeFromTop(space));
        fcLabel.setBounds(fLeftArea.removeFromTop(space));
    }
    {
        auto fLeftArea = fLabelsArea.removeFromLeft(140);

        fValueLabel.setBounds(fLeftArea.removeFromTop(space));
        fcValueLabel.setBounds(fLeftArea.removeFromTop(space));
    }
    {
        auto fLeftArea = fLabelsArea.removeFromLeft(40);

        for (auto& freqLabel : freqLabels)
        {
            freqLabel.setBounds(fLeftArea.removeFromTop(space));
        }
    }

    for (auto& freqValueLabel : freqValueLabels)
    {
        freqValueLabel.setBounds(fLabelsArea.removeFromTop(space));
    }
}

void MusicalRingModAudioProcessorEditor::timerCallback()
{
    if (*valueTreeState.getRawParameterValue(pidToggleMidiSource) == 1.0f)
    {
        lfoFreqSlider.setValue(processor.midiFreqAndOffset);
        offsetsLabel.setVisible(true);
        offsetSemitoneSlider.setVisible(true);
        offsetCentsSlider.setVisible(true);
        offsetOctaveSlider.setVisible(true);
        standardSlider.setVisible(true);
        keyboard.setVisible(true);
        setSize(800, 600);
    }
    else
    {
        offsetsLabel.setVisible(false);
        offsetSemitoneSlider.setVisible(false);
        offsetCentsSlider.setVisible(false);
        offsetOctaveSlider.setVisible(false);
        standardSlider.setVisible(false);
        keyboard.setVisible(false);
        setSize(800, 400);
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
