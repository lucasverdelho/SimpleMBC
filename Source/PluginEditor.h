/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// Colours from ableton dark theme

#define header_mid_gray = juce::Colour(0xff484848)
#define body_gray = juce::Colour(0xff373737)
#define box_gray = juce::Colour(0xff444444)
#define accent_orange = juce::Colour(0xfff39420)
#define accent_blue = juce::Colour(0xff33bfdb)


struct Placeholder : juce::Component
{
    Placeholder();

    void paint(juce::Graphics& g) override
    {
		g.fillAll(customColour);
	}
    juce::Colour customColour;
};

struct GlobalControls : juce::Component
{
    void paint(juce::Graphics& g) override;
};



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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleMBCAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer, globalControls, bandControls;
    GlobalControls globalControls;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCAudioProcessorEditor)
};
