/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// Colours from ableton dark theme

#define border_mid_gray juce::Colour(0xff484848)
#define border_dark_gray juce::Colour(0xff141414)
#define body_gray juce::Colour(0xff373737)
#define box_gray juce::Colour(0xff444444)
#define accent_orange juce::Colour(0xfff39420)
#define accent_blue juce::Colour(0xff33bfdb)


//==============================================================================

struct CompressorBandControls : juce::Component
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider ;
    RatioSlider ratioSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment,
                                releaseSliderAttachment,
                                thresholdSliderAttachment,
                                ratioSliderAttachment;

    juce::ToggleButton bypassButton, soloButton, muteButton, lowBand, midBand, highBand;

    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment,
								   soloButtonAttachment,
								   muteButtonAttachment;

    juce::Component::SafePointer<CompressorBandControls> safePtr {this};

    void updateAttachments();

};

//==============================================================================

struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    using RSWL = RotarySliderWithLabels;
    std::unique_ptr<RSWL> inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> inGainAttachment, 
                                lowMidXoverAttachment, 
                                midHighXoverAttachment, 
                                outGainAttachment;
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
    LookAndFeel lnf;

    SimpleMBCAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer;
    GlobalControls globalControls { audioProcessor.apvts };
    CompressorBandControls bandControls { audioProcessor.apvts };



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCAudioProcessorEditor)
};
