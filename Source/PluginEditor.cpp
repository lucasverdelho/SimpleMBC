/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/Params.h"


ControlBar::ControlBar()
{
    analyzerButton.setToggleState(true, juce::NotificationType::dontSendNotification);
	addAndMakeVisible(analyzerButton);

    addAndMakeVisible(globalBypassButton);
}

void ControlBar::paint(juce::Graphics& g)
{
	drawModuleBackground(g, getLocalBounds());
}

void ControlBar::resized()
{
	auto bounds = getLocalBounds();

    analyzerButton.setBounds(bounds.removeFromLeft(50).withTrimmedTop(4).withTrimmedBottom(4));

    globalBypassButton.setBounds(bounds.removeFromRight(50).withTrimmedTop(4).withTrimmedBottom(4));
}   


//==============================================================================
SimpleMBCAudioProcessorEditor::SimpleMBCAudioProcessorEditor (SimpleMBCAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&lnf);

    controlBar.analyzerButton.onClick = [this]()
    {
        auto shouldBeOn = controlBar.analyzerButton.getToggleState();
        analyzer.toggleAnalysisEnablement(shouldBeOn);
    };

    controlBar.globalBypassButton.onClick = [this]()
    {
		toggleGlobalBypassState();
	};

    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize (600, 500);

    startTimerHz(60);
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



void SimpleMBCAudioProcessorEditor::timerCallback()
{
    std::vector<float> values
    {
        audioProcessor.lowBandComp.getRMSInputLevelDb(),
        audioProcessor.lowBandComp.getRMSOutputLevelDb(),
        audioProcessor.midBandComp.getRMSInputLevelDb(),
        audioProcessor.midBandComp.getRMSOutputLevelDb(),
        audioProcessor.highBandComp.getRMSInputLevelDb(),
        audioProcessor.highBandComp.getRMSOutputLevelDb()
    };

    analyzer.update(values);
}


void SimpleMBCAudioProcessorEditor::toggleGlobalBypassState()
{
    auto shouldEnableEverything = ! controlBar.globalBypassButton.getToggleState();

    auto params = getBypassParams();

    auto bypassParamHelper = [](auto* param, bool shouldBeBypassed)
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(shouldBeBypassed ? 1.f : 0.f);
            param->endChangeGesture();
        };

    for (auto* param : params)
    {
        bypassParamHelper(param, !shouldEnableEverything);
    }
}



std::array<juce::AudioParameterBool*, 3> SimpleMBCAudioProcessorEditor::getBypassParams()
{
    using namespace Params;
    using namespace juce;
    const auto& params = Params::GetParams();
    auto& apvts = audioProcessor.apvts;

    auto boolHelper = [&apvts, &params](const auto& paramName)
    {
        auto param = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);

        return param;
    };

    auto* lowBypassParam = boolHelper(Names::Bypassed_Low_Band);
    auto* midBypassParam = boolHelper(Names::Bypassed_Mid_Band);
    auto* highBypassParam = boolHelper(Names::Bypassed_High_Band);

    return { lowBypassParam, midBypassParam, highBypassParam };
}