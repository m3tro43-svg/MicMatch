#include "PluginProcessor.h"
#include "PluginEditor.h"

MicMatchAudioProcessor::MicMatchAudioProcessor()
 : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                  .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
   parameters (*this, nullptr, "PARAMETERS", createParameterLayout()) {}

juce::AudioProcessorValueTreeState::ParameterLayout MicMatchAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterFloat>("mix", "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice>("preset", "Mic Character",
        juce::StringArray { "A - U87", "B - TLM 102", "C - C800G", "D - C414" }, 0));
    return layout;
}

bool MicMatchAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    return in == out && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

void MicMatchAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, 1, 1 };
    for (auto* bank : { &leftFilters, &rightFilters })
        for (auto& f : *bank) f.prepare (spec);
    mixSmoother.reset (sampleRate, 0.025);
    mixSmoother.setCurrentAndTargetValue (*parameters.getRawParameterValue("mix") * 0.01f);
    lastPreset = -1;
    updateFilters ((int) *parameters.getRawParameterValue("preset"));
}

void MicMatchAudioProcessor::updateFilters (int p)
{
    struct Voice { float low, body, presence, air, centre; };
    static constexpr Voice voices[] = {
        { -1.0f, -1.8f, 1.2f, 1.5f, 5200.0f },
        { -1.5f, -2.2f, 3.0f, 2.4f, 6500.0f },
        { -1.8f, -1.2f, 3.4f, 3.2f, 9000.0f },
        { -0.6f, -2.5f, 1.8f, 0.8f, 4800.0f }
    };
    p = juce::jlimit (0, 3, p);
    const auto v = voices[p];
    const std::array<juce::dsp::IIR::Coefficients<float>::Ptr, 5> coeffs = {
        juce::dsp::IIR::Coefficients<float>::makeHighPass (currentSampleRate, 28.0, 0.707),
        juce::dsp::IIR::Coefficients<float>::makeLowShelf (currentSampleRate, 145.0, 0.75, juce::Decibels::decibelsToGain(v.low)),
        juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, 310.0, 0.85, juce::Decibels::decibelsToGain(v.body)),
        juce::dsp::IIR::Coefficients<float>::makePeakFilter (currentSampleRate, v.centre, 0.8, juce::Decibels::decibelsToGain(v.presence)),
        juce::dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, 11500.0, 0.7, juce::Decibels::decibelsToGain(v.air))
    };
    for (int i = 0; i < 5; ++i)
    {
        leftFilters[(size_t)i].coefficients = coeffs[(size_t)i];
        rightFilters[(size_t)i].coefficients = coeffs[(size_t)i];
    }
    lastPreset = p;
}

void MicMatchAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int preset = (int) *parameters.getRawParameterValue("preset");
    if (preset != lastPreset) updateFilters (preset);
    mixSmoother.setTargetValue (*parameters.getRawParameterValue("mix") * 0.01f);
    const int channels = buffer.getNumChannels();
    const int samples = buffer.getNumSamples();
    for (int n = 0; n < samples; ++n)
    {
        const float mix = mixSmoother.getNextValue();
        for (int ch = 0; ch < channels; ++ch)
        {
            auto* data = buffer.getWritePointer (ch);
            auto& bank = (ch == 0 ? leftFilters : rightFilters);
            const float dry = data[n];
            float wet = dry;
            for (auto& filter : bank) wet = filter.processSample (wet);
            data[n] = dry + (wet - dry) * mix;
        }
    }
}

juce::AudioProcessorEditor* MicMatchAudioProcessor::createEditor() { return new MicMatchAudioProcessorEditor (*this); }
void MicMatchAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, dest);
}
void MicMatchAudioProcessor::setStateInformation (const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, size));
    if (xml && xml->hasTagName (parameters.state.getType())) parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new MicMatchAudioProcessor(); }
