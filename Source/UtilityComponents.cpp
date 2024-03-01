/*
  ==============================================================================

    UtilityComponents.cpp
    Created: 1 Mar 2024 12:22:22am
    Author:  despacito

  ==============================================================================
*/

#include "UtilityComponents.h"


void Placeholder::paint(juce::Graphics& g)
{
    g.fillAll(customColour);
}



Placeholder::Placeholder()
{
    juce::Random r;
    customColour = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}



RotarySlider::RotarySlider() :
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox)
{}