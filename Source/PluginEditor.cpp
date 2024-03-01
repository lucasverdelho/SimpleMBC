/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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





































//==============================================================================
void RotarySliderWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    auto bounds = getLocalBounds();

    g.setColour(accent_orange);

    g.drawFittedText(getName(),
                     bounds.removeFromTop(getTextHeight() + 2),
                     juce::Justification::centredBottom,
                     1);


    //    g.setColour(Colours::red);
    //    g.drawRect(getLocalBounds());
    //    g.setColour(Colours::yellow);
    //    g.drawRect(sliderBounds);

    getLookAndFeel().drawRotarySlider(g,
        sliderBounds.getX(),
        sliderBounds.getY(),
        sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
        startAng,
        endAng,
        *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;

    g.setColour(Colour(0u, 172u, 1u));
    g.setFont(getTextHeight());

    auto numChoices = labels.size();
    for (int i = 0; i < numChoices; ++i)
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);

        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);

        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);

        Rectangle<float> r;
        auto str = labels[i].label;
        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight());

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }

}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(getTextHeight() * 1.5);

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 1.5;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(bounds.getY());

    return r;

}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
        return choiceParam->getCurrentChoiceName();

    juce::String str;
    bool addK = false;

    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();

        addK = truncateKiloValue(val);

        str = juce::String(val, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse; //this shouldn't happen!
    }

    if (suffix.isNotEmpty())
    {
        str << " ";
        if (addK)
            str << "k";

        str << suffix;
    }

    return str;
}



void RotarySliderWithLabels::changeParam(juce::RangedAudioParameter* p)
{
	param = p;
	repaint();
}
















//===================================================================================



juce::String RatioSlider::getDisplayString() const
{
    auto choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param);
    jassert(choiceParam != nullptr);

    auto currentChoice = choiceParam->getCurrentChoiceName();

    if (currentChoice.contains(".0"))
        currentChoice = currentChoice.dropLastCharacters(2);

    currentChoice << ":1";

    return currentChoice;
}












//==============================================================================

Placeholder::Placeholder()
{
    juce::Random r;
    customColour = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}



















//==============================================================================

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



void drawModulBackground(juce::Graphics &g, juce::Rectangle<int> bounds) 
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
            names = std::vector<Names> { Names::Attack_Low_Band,
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
            names = std::vector<Names> { Names::Attack_Mid_Band,
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
            names = std::vector<Names> { Names::Attack_High_Band,
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



























//==============================================================================

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
