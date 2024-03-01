/*
  ==============================================================================

    CompressorBandControls.cpp
    Created: 1 Mar 2024 12:33:07am
    Author:  despacito

  ==============================================================================
*/

#include "CompressorBandControls.h"


CompressorBandControls::CompressorBandControls(juce::AudioProcessorValueTreeState& apv) :
    apvts(apv),
    attackSlider(nullptr, "ms", "Attack"),
    releaseSlider(nullptr, "ms", "Release"),
    thresholdSlider(nullptr, "dB", "Threshold"),
    ratioSlider(nullptr, "")
{
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);



    bypassButton.setName("X");
    soloButton.setName("S");
    muteButton.setName("M");

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

    lowBand.setName("Low");
    midBand.setName("Mid");
    highBand.setName("High");

    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);

    lowBand.setRadioGroupId(1);
    midBand.setRadioGroupId(1);
    highBand.setRadioGroupId(1);

    auto buttonSwitcher = [safePtr = this->safePtr]()
        {
            if (auto* comp = safePtr.getComponent())
            {
                comp->updateAttachments();
            }
        };

    lowBand.onClick = buttonSwitcher;
    midBand.onClick = buttonSwitcher;
    highBand.onClick = buttonSwitcher;

    lowBand.setToggleState(true, juce::NotificationType::dontSendNotification);

    updateAttachments();

    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);


}



void CompressorBandControls::resized()
{
    using namespace juce;
    auto bounds = getLocalBounds().reduced(5);

    auto createBandButtonControlBox = [](std::vector<Component*> comps)
        {
            FlexBox flexBox;
            flexBox.flexDirection = FlexBox::Direction::column;
            flexBox.flexWrap = FlexBox::Wrap::noWrap;

            auto spacer = FlexItem().withHeight(1);

            for (auto* comp : comps)
            {
                flexBox.items.add(spacer);
                flexBox.items.add(FlexItem(*comp).withFlex(1.f));
            }

            flexBox.items.add(spacer);

            return flexBox;
        };

    auto bandButtonControlBox = createBandButtonControlBox({ &bypassButton, &soloButton, &muteButton });
    auto bandSelectControlBox = createBandButtonControlBox({ &lowBand, &midBand, &highBand });

    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;

    auto spacer = FlexItem().withWidth(4.f);

    flexBox.items.add(FlexItem(bandSelectControlBox).withWidth(50.f));

    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.f));
    flexBox.items.add(spacer);

    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.f));
    flexBox.items.add(spacer);


    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
    flexBox.items.add(spacer);


    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.f));

    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandButtonControlBox).withWidth(40.f));

    flexBox.performLayout(bounds);

}








void CompressorBandControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    drawModulBackground(g, bounds);
}



void CompressorBandControls::updateAttachments()
{
    enum BandType
    {
        Low,
        Mid,
        High
    };

    BandType bandType = [this]()
        {
            if (lowBand.getToggleState())
                return BandType::Low;
            if (midBand.getToggleState())
                return BandType::Mid;

            return BandType::High;
        }();

        using namespace Params;
        std::vector<Names> names;

        switch (bandType)
        {
        case Low:
        {
            names = std::vector<Names>{ Names::Attack_Low_Band,
                      Names::Release_Low_Band,
                      Names::Threshold_Low_Band,
                      Names::Ratio_Low_Band,
                      Names::Mute_Low_Band,
                      Names::Solo_Low_Band,
                      Names::Bypassed_Low_Band };
            break;
        }

        case Mid:
        {
            names = std::vector<Names>{ Names::Attack_Mid_Band,
                      Names::Release_Mid_Band,
                      Names::Threshold_Mid_Band,
                      Names::Ratio_Mid_Band,
                      Names::Mute_Mid_Band,
                      Names::Solo_Mid_Band,
                      Names::Bypassed_Mid_Band };
            break;
        }

        case High:
        {
            names = std::vector<Names>{ Names::Attack_High_Band,
                      Names::Release_High_Band,
                      Names::Threshold_High_Band,
                      Names::Ratio_High_Band,
                      Names::Mute_High_Band,
                      Names::Solo_High_Band,
                      Names::Bypassed_High_Band };
            break;
        }
        }

        enum Pos
        {
            Attack,
            Release,
            Threshold,
            Ratio,
            Mute,
            Solo,
            Bypass
        };

        const auto& params = GetParams();

        auto getParamHelper = [&params, &apvts = this->apvts, &names](const auto& pos) -> auto&
            {
                return getParam(apvts, params, names.at(pos));
            };

        attackSliderAttachment.reset();
        releaseSliderAttachment.reset();
        thresholdSliderAttachment.reset();
        ratioSliderAttachment.reset();
        muteButtonAttachment.reset();
        soloButtonAttachment.reset();
        bypassButtonAttachment.reset();

        auto& attackParam = getParamHelper(Pos::Attack);
        //addLabelPairs(attackSlider.labels, attackParam, "ms");
        attackSlider.changeParam(&attackParam);

        auto& releaseParam = getParamHelper(Pos::Release);
        //addLabelPairs(releaseSlider.labels, releaseParam, "ms");
        releaseSlider.changeParam(&releaseParam);

        auto& thresholdParam = getParamHelper(Pos::Threshold);
        //addLabelPairs(thresholdSlider.labels, thresholdParam, "dB");
        thresholdSlider.changeParam(&thresholdParam);

        auto& ratioParamRap = getParamHelper(Pos::Ratio);
        //ratioSlider.labels.clear();
        //ratioSlider.labels.add({0.f, "1:1" });
        auto ratioParam = dynamic_cast<juce::AudioParameterChoice*>(&ratioParamRap);
        //ratioSlider.labels.add({ 1.f, juce::String(ratioParam->getMax(), 1) + ":1" });
        ratioSlider.changeParam(ratioParam);

        auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment, const auto& name, auto& slider)
            {
                makeAttachment(attachment, apvts, params, name, slider);
            };

        makeAttachmentHelper(attackSliderAttachment, names[Pos::Attack], attackSlider);
        makeAttachmentHelper(releaseSliderAttachment, names[Pos::Release], releaseSlider);
        makeAttachmentHelper(thresholdSliderAttachment, names[Pos::Threshold], thresholdSlider);
        makeAttachmentHelper(ratioSliderAttachment, names[Pos::Ratio], ratioSlider);
        makeAttachmentHelper(bypassButtonAttachment, names[Pos::Bypass], bypassButton);
        makeAttachmentHelper(soloButtonAttachment, names[Pos::Solo], soloButton);
        makeAttachmentHelper(muteButtonAttachment, names[Pos::Mute], muteButton);
}

