/*
  ==============================================================================

    CompressorBand.cpp
    Created: 1 Mar 2024 12:40:57am
    Author:  despacito

  ==============================================================================
*/

#include "CompressorBand.h"

void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
}

void CompressorBand::updateCompressorSettings()
{
    compressor.setAttack(*attack);
    compressor.setRelease(*release);
    compressor.setThreshold(*threshold);
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

void CompressorBand::process(juce::AudioBuffer<float>& buffer)
{
    auto preRMS = computeRMSLevel(buffer);

    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    context.isBypassed = bypassed->get();

    compressor.process(context);

    auto postRMS = computeRMSLevel(buffer);

    auto converToDb = [](auto input) {
        return juce::Decibels::gainToDecibels(input);
    };

    rmsInputLevelDb.store(converToDb(preRMS));
    rmsOutputLevelDb.store(converToDb(postRMS));
}
