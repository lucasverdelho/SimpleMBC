/*
  ==============================================================================

    UtilityComponents.h
    Created: 1 Mar 2024 12:22:22am
    Author:  despacito

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct Placeholder : juce::Component
{
    Placeholder();

    void paint(juce::Graphics& g) override;
    juce::Colour customColour;
};


struct RotarySlider : juce::Slider
{
    RotarySlider();
};
