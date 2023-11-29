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






void LookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    auto enabled = slider.isEnabled();

    g.setColour(enabled ? body_gray : Colours::darkgrey);
    g.fillEllipse(bounds);

    g.setColour(enabled ? accent_blue : Colours::grey);
    g.drawEllipse(bounds, 2.5f);

    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
        Path p;

        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);

        p.addRoundedRectangle(r, 2.f);

        jassert(rotaryStartAngle < rotaryEndAngle);

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

        g.fillPath(p);

        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);

        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());

        //g.setColour(enabled ? Colours::black : Colours::darkgrey);
        //g.fillRect(r);

        g.setColour(enabled ? Colours::white : Colours::lightgrey);
        // Create a font with bold style
        //juce::Font boldFont(20.0f, juce::Font::bold);
        juce::Font customFont("Cascadia Mono", 13.0f, juce::Font::plain);
        // Create a label with bold text
        g.setFont(customFont);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics& g,
    juce::ToggleButton& toggleButton,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    using namespace juce;

    if (auto* pb = dynamic_cast<PowerButton*>(&toggleButton))
    {
        Path powerButton;

        auto bounds = toggleButton.getLocalBounds();

        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

        float ang = 30.f; //30.f;

        size -= 6;

        powerButton.addCentredArc(r.getCentreX(),
            r.getCentreY(),
            size * 0.5,
            size * 0.5,
            0.f,
            degreesToRadians(ang),
            degreesToRadians(360.f - ang),
            true);

        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());

        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

        auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);

        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);
    }
    else if (auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton))
    {
        auto color = !toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);

        g.setColour(color);

        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);

        g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
    }

    else
    {
        auto bounds = toggleButton.getLocalBounds().reduced(2);

        auto buttonIsOn = toggleButton.getToggleState();

        // Rectangle itself
        g.setColour(buttonIsOn ? accent_orange : Colours::darkgrey);
        g.fillRect(bounds);

        // Contour
        g.setColour(Colours::black);
        g.drawRect(bounds, 1.f);

        // Text
        g.setColour(buttonIsOn ? Colours::white : Colours::black);
        g.drawFittedText(toggleButton.getName(),
            			 bounds,
            			 Justification::centred,
            			 1);
    }
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

    using namespace Params;
    const auto& params = GetParams();

    auto getParamHelper = [&params, &apvts = this->apvts](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };

    attackSlider.changeParam(&getParamHelper(Names::Attack_Mid_Band));
    releaseSlider.changeParam(&getParamHelper(Names::Release_Mid_Band));
    thresholdSlider.changeParam(&getParamHelper(Names::Threshold_Mid_Band));
    ratioSlider.changeParam(&getParamHelper(Names::Ratio_Mid_Band));

    // LABELS MIGHT NOT INCLUDE
    //addLabelPairs(attackSlider.labels, getParamHelper(Names::Attack_Mid_Band), "ms");
    //addLabelPairs(releaseSlider.labels, getParamHelper(Names::Release_Mid_Band), "ms");
    //addLabelPairs(thresholdSlider.labels, getParamHelper(Names::Threshold_Mid_Band), "dB");
    //addLabelPairs(ratioSlider.labels, getParamHelper(Names::Ratio_Mid_Band), "");

    //ratioSlider.labels.add({0.f, "1:1" });
    //auto ratioParam = dynamic_cast<juce::AudioParameterFloat*>(&getParamHelper(Names::Ratio_Mid_Band));
    //ratioSlider.labels.add({ 1.f, juce::String(ratioParam->getMax(), 1) + ":1" });


    auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment, const auto& name, auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };


    makeAttachmentHelper(attackSliderAttachment, Names::Attack_Mid_Band, attackSlider);
    makeAttachmentHelper(releaseSliderAttachment, Names::Release_Mid_Band, releaseSlider);
    makeAttachmentHelper(thresholdSliderAttachment, Names::Threshold_Mid_Band, thresholdSlider);
    makeAttachmentHelper(ratioSliderAttachment, Names::Ratio_Mid_Band, ratioSlider);


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


    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;

    auto spacer = FlexItem().withWidth(4.f);
    auto endCap = FlexItem().withWidth(6.f);

    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(attackSlider).withFlex(1.f));
    flexBox.items.add(spacer);

    flexBox.items.add(FlexItem(releaseSlider).withFlex(1.f));
    flexBox.items.add(spacer);


    flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.f));
    flexBox.items.add(spacer);


    flexBox.items.add(FlexItem(ratioSlider).withFlex(1.f));
    /*flexBox.items.add(endCap);*/

    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(bandButtonControlBox).withWidth(30.f));

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
