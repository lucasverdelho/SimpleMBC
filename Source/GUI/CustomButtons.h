/*
  ==============================================================================

    CustomButtons.h
    Created: 1 Mar 2024 12:18:19am
    Author:  despacito

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct PowerButton : juce::ToggleButton { };


struct AnalyzerButton : juce::ToggleButton { 
    void resized() override;

    juce::Path randomPath;
};

