/*
  ==============================================================================

    PathProducer.h
    Created: 1 Mar 2024 1:24:01pm
    Author:  despacito

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "FFTDataGenerator.h"
#include "AnalyzerPathGenerator.h"

#include "../PluginProcessor.h"

struct PathProducer
{
    PathProducer(SingleChannelSampleFifo<SimpleMBCAudioProcessor::BlockType>& scsf) :
        leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; }

    void updateNegativeInfinity(float nf) { negativeInfinity = nf; }

private:
    SingleChannelSampleFifo<SimpleMBCAudioProcessor::BlockType>* leftChannelFifo;

    juce::AudioBuffer<float> monoBuffer;

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;

    float negativeInfinity{ -48.0f };
};



