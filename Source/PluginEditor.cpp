/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


Placeholder::Placeholder()
{
    juce::Random r;
    customColour = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}



//==============================================================================
SimpleMBCAudioProcessorEditor::SimpleMBCAudioProcessorEditor (SimpleMBCAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize (600, 500);
}

SimpleMBCAudioProcessorEditor::~SimpleMBCAudioProcessorEditor()
{
}
















//==============================================================================
void SimpleMBCAudioProcessorEditor::paint (juce::Graphics& g)
{
    resized();
}
































void SimpleMBCAudioProcessorEditor::resized()
{
    
    auto bounds = getLocalBounds();

    controlBar.setBounds(bounds.removeFromTop(32));

    bandControls.setBounds(bounds.removeFromBottom(135));

    analyzer.setBounds(bounds.removeFromTop(225));

    globalControls.setBounds(bounds);
}
