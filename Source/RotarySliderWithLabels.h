/*
  ==============================================================================

    RotarySliderWithLabels.h
    Created: 1 Mar 2024 12:08:56am
    Author:  despacito

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



#define border_mid_gray juce::Colour(0xff484848)
#define border_dark_gray juce::Colour(0xff141414)
#define body_gray juce::Colour(0xff373737)
#define box_gray juce::Colour(0xff444444)
#define accent_orange juce::Colour(0xfff39420)
#define accent_blue juce::Colour(0xff33bfdb)








struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter* rap,
        const juce::String& unitSuffix,
        const juce::String& title) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(rap),
        suffix(unitSuffix)
    {
        setName(title);
        //setLookAndFeel(&lnf);
    }

    //~RotarySliderWithLabels()
    //{
    //    setLookAndFeel(nullptr);
    //}

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    virtual juce::String getDisplayString() const;

    void changeParam(juce::RangedAudioParameter* p);



protected:
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

struct RatioSlider : RotarySliderWithLabels
{
    RatioSlider(juce::RangedAudioParameter* rap, const juce::String& unitSuffix) :
        RotarySliderWithLabels(rap, unitSuffix, "Ratio") {}

    juce::String getDisplayString() const override;
};

struct PowerButton : juce::ToggleButton { };
