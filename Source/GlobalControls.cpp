/*
  ==============================================================================

    GlobalControls.cpp
    Created: 1 Mar 2024 12:35:23am
    Author:  despacito

  ==============================================================================
*/

#include "GlobalControls.h"


GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();

    auto getParamHelper = [&params, &apvts](const auto& name) -> auto&
        {
            return getParam(apvts, params, name);
        };


    auto& gainInParam = getParamHelper(Names::Gain_In);
    auto& lowMidXoverParam = getParamHelper(Names::Low_Mid_Crossover_Freq);
    auto& midHighXoverParam = getParamHelper(Names::Mid_High_Crossover_Freq);
    auto& gainOutParam = getParamHelper(Names::Gain_Out);


    inGainSlider = std::make_unique<RSWL>(&gainInParam, "dB", "Input Gain");
    lowMidXoverSlider = std::make_unique<RSWL>(&lowMidXoverParam, "Hz", "Low-Mid X-Over");
    midHighXoverSlider = std::make_unique<RSWL>(&midHighXoverParam, "Hz", "Mid-High X-Over");
    outGainSlider = std::make_unique<RSWL>(&gainOutParam, "dB", "Output Gain");

    auto makeAttachmentHelper = [&params, &apvts](auto& attachment, const auto& name, auto& slider)
        {
            makeAttachment(attachment, apvts, params, name, slider);
        };

    makeAttachmentHelper(inGainAttachment, Names::Gain_In, *inGainSlider);
    makeAttachmentHelper(lowMidXoverAttachment, Names::Low_Mid_Crossover_Freq, *lowMidXoverSlider);
    makeAttachmentHelper(midHighXoverAttachment, Names::Mid_High_Crossover_Freq, *midHighXoverSlider);
    makeAttachmentHelper(outGainAttachment, Names::Gain_Out, *outGainSlider);


    // ADD LABELS TO SLIDERS MAY NOT INCLUDE THIS IN FINAL VERSION
    //addLabelPairs(inGainSlider->labels,gainInParam, "dB");
    //addLabelPairs(lowMidXoverSlider->labels, lowMidXoverParam, "Hz");
    //addLabelPairs(midHighXoverSlider->labels, midHighXoverParam, "Hz");
    //addLabelPairs(outGainSlider->labels, gainOutParam, "dB");



    addAndMakeVisible(*inGainSlider);
    addAndMakeVisible(*lowMidXoverSlider);
    addAndMakeVisible(*midHighXoverSlider);
    addAndMakeVisible(*outGainSlider);



}


void GlobalControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    drawModulBackground(g, bounds);
}




void GlobalControls::resized()
{
    using namespace juce;
    auto bounds = getLocalBounds().reduced(5);

    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;

    auto spacer = FlexItem().withWidth(4.f);
    auto endCap = FlexItem().withWidth(6.f);

    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(*inGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);

    flexBox.items.add(FlexItem(*lowMidXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);


    flexBox.items.add(FlexItem(*midHighXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);


    flexBox.items.add(FlexItem(*outGainSlider).withFlex(1.f));
    flexBox.items.add(endCap);


    flexBox.performLayout(bounds);

}




