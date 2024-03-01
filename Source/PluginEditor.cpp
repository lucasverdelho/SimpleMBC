/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
SimpleMBCAudioProcessorEditor::SimpleMBCAudioProcessorEditor (SimpleMBCAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&lnf);


    //addAndMakeVisible(controlBar);
    //addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize (600, 500);
}

SimpleMBCAudioProcessorEditor::~SimpleMBCAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
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
