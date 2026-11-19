cmake_minimum_required(VERSION 3.22)
project(MicMatch VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(MicMatch
    COMPANY_NAME "MicMatch Audio"
    BUNDLE_ID "audio.micmatch.micmatch"
    PLUGIN_MANUFACTURER_CODE MchA
    PLUGIN_CODE McM1
    FORMATS VST3 Standalone
    PRODUCT_NAME "MicMatch")

target_sources(MicMatch PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

target_compile_features(MicMatch PRIVATE cxx_std_17)
target_compile_definitions(MicMatch PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(MicMatch PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)
