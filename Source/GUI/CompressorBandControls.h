/*
  ==============================================================================

    CompressorBandControls.h
    Created: 1 Mar 2024 12:33:07am
    Author:  despacito

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotarySliderWithLabels.h"



struct CompressorBandControls : juce::Component
{
    CompressorBandControls(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;
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

    juce::Component::SafePointer<CompressorBandControls> safePtr{ this };

    void updateAttachments();

};