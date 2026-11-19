#pragma once
#include <JuceHeader.h>
#include <array>

class MicMatchAudioProcessor final : public juce::AudioProcessor
{
public:
    MicMatchAudioProcessor();
    ~MicMatchAudioProcessor() override = default;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "MicMatch"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    std::array<Filter, 5> leftFilters, rightFilters;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mixSmoother;
    double currentSampleRate = 44100.0;
    int lastPreset = -1;
    void updateFilters (int preset);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicMatchAudioProcessor)
};
