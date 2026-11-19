#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class MicMatchAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit MicMatchAudioProcessorEditor (MicMatchAudioProcessor&);
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    MicMatchAudioProcessor& processor;
    juce::Slider mix;
    juce::Label title, subtitle, mixLabel, status;
    std::array<juce::TextButton, 4> presetButtons;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> presetAttachment;
    juce::ComboBox hiddenPreset;
    void refreshPresetButtons();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicMatchAudioProcessorEditor)
};
