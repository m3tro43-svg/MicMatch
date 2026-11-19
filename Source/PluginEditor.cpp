#include "PluginEditor.h"

namespace { constexpr juce::uint32 bg = 0xff111318, panel = 0xff1b1f26, muted = 0xff858d9a, accent = 0xffc5d2e2; }

MicMatchAudioProcessorEditor::MicMatchAudioProcessorEditor (MicMatchAudioProcessor& p)
 : AudioProcessorEditor (&p), processor (p)
{
    setSize (430, 390);
    title.setText ("MIC//MATCH", juce::dontSendNotification);
    title.setFont (juce::Font (juce::FontOptions (23.0f, juce::Font::bold)));
    title.setColour (juce::Label::textColourId, juce::Colour (0xfff1f3f6));
    title.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (title);
    subtitle.setText ("WA-47jr  /  MICROPHONE CHARACTER", juce::dontSendNotification);
    subtitle.setFont (juce::Font (juce::FontOptions (10.0f)));
    subtitle.setColour (juce::Label::textColourId, juce::Colour (muted));
    subtitle.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (subtitle);

    mix.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    mix.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 76, 22);
    mix.setRange (0.0, 100.0, 0.1);
    mix.setTextValueSuffix (" %");
    mix.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (accent));
    mix.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (panel));
    mix.setColour (juce::Slider::thumbColourId, juce::Colour (0xfff5f7fa));
    mix.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe9edf3));
    mix.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (panel));
    addAndMakeVisible (mix);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, "mix", mix);

    mixLabel.setText ("MIX", juce::dontSendNotification);
    mixLabel.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    mixLabel.setColour (juce::Label::textColourId, juce::Colour (muted));
    mixLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (mixLabel);

    hiddenPreset.addItemList ({ "A - U87", "B - TLM 102", "C - C800G", "D - C414" }, 1);
    hiddenPreset.setVisible (false);
    addChildComponent (hiddenPreset);
    presetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (processor.parameters, "preset", hiddenPreset);
    const juce::String labels[] = { "A", "B", "C", "D" };
    const juce::String names[] = { "U87", "TLM 102", "C800G", "C414" };
    for (int i = 0; i < 4; ++i)
    {
        presetButtons[(size_t)i].setButtonText (labels[i] + "\n" + names[i]);
        presetButtons[(size_t)i].setClickingTogglesState (false);
        presetButtons[(size_t)i].onClick = [this, i] { hiddenPreset.setSelectedId (i + 1, juce::sendNotification); refreshPresetButtons(); };
        addAndMakeVisible (presetButtons[(size_t)i]);
    }
    status.setText ("MICROPHONE VOICING  •  EQ PROFILE", juce::dontSendNotification);
    status.setFont (juce::Font (juce::FontOptions (9.0f)));
    status.setColour (juce::Label::textColourId, juce::Colour (muted));
    status.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (status);
    refreshPresetButtons();
}

void MicMatchAudioProcessorEditor::refreshPresetButtons()
{
    const int selected = hiddenPreset.getSelectedId();
    for (int i = 0; i < 4; ++i)
    {
        auto& b = presetButtons[(size_t)i];
        const bool active = selected == i + 1;
        b.setColour (juce::TextButton::buttonColourId, active ? juce::Colour (0xffdce5ef) : juce::Colour (panel));
        b.setColour (juce::TextButton::textColourOffId, active ? juce::Colour (bg) : juce::Colour (0xffe5e9ef));
        b.setColour (juce::TextButton::textColourOnId, juce::Colour (bg));
        b.setLookAndFeel (nullptr);
    }
}

void MicMatchAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (bg));
    g.setColour (juce::Colour (0xff272c35));
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (12.0f), 15.0f, 1.0f);
    g.setColour (juce::Colour (0xff252a33));
    g.drawLine (28.0f, 77.0f, (float)getWidth() - 28.0f, 77.0f, 1.0f);
}

void MicMatchAudioProcessorEditor::resized()
{
    title.setBounds (25, 22, getWidth()-50, 30);
    subtitle.setBounds (25, 53, getWidth()-50, 16);
    mix.setBounds (getWidth()/2 - 92, 91, 184, 190);
    mixLabel.setBounds (25, 275, getWidth()-50, 16);
    const int gap = 9, w = (getWidth() - 64 - 3*gap) / 4;
    for (int i = 0; i < 4; ++i) presetButtons[(size_t)i].setBounds (32 + i*(w+gap), 300, w, 52);
    status.setBounds (20, 358, getWidth()-40, 15);
}
