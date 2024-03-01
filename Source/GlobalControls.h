/*
  ==============================================================================

    GlobalControls.h
    Created: 1 Mar 2024 12:35:23am
    Author:  despacito

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


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