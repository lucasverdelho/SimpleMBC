/*
  ==============================================================================

    CustomButtons.cpp
    Created: 1 Mar 2024 12:18:19am
    Author:  despacito

  ==============================================================================
*/

#include "CustomButtons.h"

void AnalyzerButton::resized() 
{
    auto bounds = getLocalBounds();
    auto insetRect = bounds.reduced(4);

    randomPath.clear();

    juce::Random r;

    randomPath.startNewSubPath(insetRect.getX(),
        insetRect.getY() + insetRect.getHeight() * r.nextFloat());

    for (auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2)
    {
        randomPath.lineTo(x,
            insetRect.getY() + insetRect.getHeight() * r.nextFloat());
    }
}

juce::Path randomPath;