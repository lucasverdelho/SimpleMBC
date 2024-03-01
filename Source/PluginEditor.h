#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "GUI/LookAndFeel.h"
#include "GUI/GlobalControls.h"
#include "GUI/CompressorBandControls.h"
#include "GUI/UtilityComponents.h"
#include "GUI/SpectrumAnalyzer.h"
#include "GUI/CustomButtons.h"

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
class SimpleMBCAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    SimpleMBCAudioProcessorEditor (SimpleMBCAudioProcessor&);
    ~SimpleMBCAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    LookAndFeel lnf;

    SimpleMBCAudioProcessor& audioProcessor;

    Placeholder controlBar;
    GlobalControls globalControls { audioProcessor.apvts };
    CompressorBandControls bandControls { audioProcessor.apvts };
    SpectrumAnalyzer analyzer { audioProcessor };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCAudioProcessorEditor)
};
