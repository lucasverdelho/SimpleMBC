/*
  ==============================================================================

    Utilities.h
    Created: 1 Mar 2024 12:26:26am
    Author:  despacito

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


#define border_mid_gray juce::Colour(0xff484848)
#define border_dark_gray juce::Colour(0xff141414)
#define body_gray juce::Colour(0xff373737)
#define box_gray juce::Colour(0xff444444)
#define accent_orange juce::Colour(0xfff39420)
#define accent_blue juce::Colour(0xff33bfdb)




enum FFTOrder
{
    order2048 = 11,
    order4096 = 12,
    order8192 = 13
};




template<typename Attachment, typename APVTS, typename Params, typename ParamName, typename SliderType>
void makeAttachment(std::unique_ptr<Attachment>& attachment,
    APVTS& apvts,
    const Params& params,
    const ParamName& name,
    SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}



template< typename APVTS, typename Params, typename Name>
juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert(param != nullptr);

    return *param;
}


juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix);



template<typename T>
bool truncateKiloValue(T& value)
{
    if (value > static_cast<T>(999))
    {
        value /= static_cast<T>(1000);
        return true;
    }
    return false;
}




template< typename Labels, typename ParamType, typename SuffixType>
void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix)
{
    labels.clear();
    labels.add({ 0.f, getValString(param, true, suffix) });
    labels.add({ 1.f, getValString(param, false, suffix) });
}


void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
