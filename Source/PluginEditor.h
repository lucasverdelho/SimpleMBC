/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "GlobalControls.h"
#include "CompressorBandControls.h"
#include "UtilityComponents.h"

// Colours from ableton dark theme

#define border_mid_gray juce::Colour(0xff484848)
#define border_dark_gray juce::Colour(0xff141414)
#define body_gray juce::Colour(0xff373737)
#define box_gray juce::Colour(0xff444444)
#define accent_orange juce::Colour(0xfff39420)
#define accent_blue juce::Colour(0xff33bfdb)

//==============================================================================
/**
*/
class SimpleMBCAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleMBCAudioProcessorEditor (SimpleMBCAudioProcessor&);
    ~SimpleMBCAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    LookAndFeel lnf;

    SimpleMBCAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer;
    GlobalControls globalControls { audioProcessor.apvts };
    CompressorBandControls bandControls { audioProcessor.apvts };



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCAudioProcessorEditor)
};
