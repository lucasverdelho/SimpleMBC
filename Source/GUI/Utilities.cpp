/*
  ==============================================================================

    Utilities.cpp
    Created: 1 Mar 2024 12:26:26am
    Author:  despacito

  ==============================================================================
*/

#include "Utilities.h"



juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix)
{
    juce::String str;

    auto val = getLow ? param.getNormalisableRange().start : param.getNormalisableRange().end;

    bool useK = truncateKiloValue(val);
    str << val;

    if (useK)
        str << "k";

    str << suffix;

    return str;
}


juce::Rectangle<int> drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    // Draw the background
    g.setColour(border_mid_gray);
    g.fillAll();

    auto localBounds = bounds;

    // Draw the body
    bounds.reduce(3, 3);
    g.setColour(body_gray);
    g.fillRoundedRectangle(bounds.toFloat(), 3.f);

    // Border between components
    g.setColour(border_mid_gray); // Mid gray for testing
    g.drawRect(localBounds);

    return bounds;
}
