/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/Params.h"

//==============================================================================
SimpleMBCAudioProcessor::SimpleMBCAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    using namespace Params;
    const auto& params = GetParams();

    auto floatHelper = [&apvts = this ->apvts, &params](auto &param, const auto& paramName)
    {
         param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
         jassert(param != nullptr);
    };

    floatHelper(lowBandComp.threshold,  Names::Threshold_Low_Band);
    floatHelper(lowBandComp.attack,     Names::Attack_Low_Band);
    floatHelper(lowBandComp.release,    Names::Release_Low_Band);

    floatHelper(midBandComp.threshold,  Names::Threshold_Mid_Band);
    floatHelper(midBandComp.attack,     Names::Attack_Mid_Band);
    floatHelper(midBandComp.release,    Names::Release_Mid_Band);

    floatHelper(highBandComp.threshold, Names::Threshold_High_Band);
    floatHelper(highBandComp.attack,    Names::Attack_High_Band);
    floatHelper(highBandComp.release,   Names::Release_High_Band);


    auto choiceHelper = [&apvts = this ->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    choiceHelper(lowBandComp.ratio,     Names::Ratio_Low_Band);
    choiceHelper(midBandComp.ratio,     Names::Ratio_Mid_Band);
    choiceHelper(highBandComp.ratio,    Names::Ratio_High_Band);

    auto boolHelper = [&apvts = this ->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };

    boolHelper(lowBandComp.bypassed,    Names::Bypassed_Low_Band);
    boolHelper(midBandComp.bypassed,    Names::Bypassed_Mid_Band);
    boolHelper(highBandComp.bypassed,   Names::Bypassed_High_Band);

    boolHelper(lowBandComp.mute,       Names::Mute_Low_Band);
    boolHelper(midBandComp.mute,       Names::Mute_Mid_Band);
    boolHelper(highBandComp.mute,      Names::Mute_High_Band);

    boolHelper(lowBandComp.solo,      Names::Solo_Low_Band);
    boolHelper(midBandComp.solo,      Names::Solo_Mid_Band);
    boolHelper(highBandComp.solo,     Names::Solo_High_Band);

    floatHelper(lowMidCrossover,    Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover,   Names::Mid_High_Crossover_Freq);

    floatHelper(inputGainParam,             Names::Gain_In);
    floatHelper(outputGainParam,            Names::Gain_Out);


    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);

    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

}

SimpleMBCAudioProcessor::~SimpleMBCAudioProcessor()
{
}


//==============================================================================
const juce::String SimpleMBCAudioProcessor::getName() const
{
    return JucePlugin_Name;
}


bool SimpleMBCAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}


bool SimpleMBCAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}


bool SimpleMBCAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleMBCAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}


int SimpleMBCAudioProcessor::getNumPrograms()
{
    return 1; 
}


int SimpleMBCAudioProcessor::getCurrentProgram()
{
    return 0;
}


void SimpleMBCAudioProcessor::setCurrentProgram (int index)
{
}


const juce::String SimpleMBCAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleMBCAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void SimpleMBCAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    spec.sampleRate = sampleRate;

    for (auto& comp : compressors)
        comp.prepare(spec);
	

    LP1.prepare(spec);
    HP1.prepare(spec);

    AP2.prepare(spec);

    LP2.prepare(spec);
    HP2.prepare(spec);


    inputGain.prepare(spec);
    outputGain.prepare(spec);

    inputGain.setRampDurationSeconds(0.05);
    outputGain.setRampDurationSeconds(0.05);


    for (auto& buffer : filterBuffers)
	{
		buffer.setSize(spec.numChannels, samplesPerBlock);
	}


    leftChannelFifo.prepare(samplesPerBlock);
	rightChannelFifo.prepare(samplesPerBlock);

}


void SimpleMBCAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleMBCAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif





void SimpleMBCAudioProcessor::updateState()
{
    for (auto& comp : compressors)
        comp.updateCompressorSettings();

    auto lowMidCutoff = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoff);
    HP1.setCutoffFrequency(lowMidCutoff);

    auto midHighCutoffFreq = midHighCrossover->get();
    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);

    inputGain.setGainDecibels(inputGainParam->get());
    outputGain.setGainDecibels(outputGainParam->get());


}



void SimpleMBCAudioProcessor::splitBands(juce::AudioBuffer<float>& inputbuffer)
{
    for (auto& fb : filterBuffers)
    {
        fb = inputbuffer;
    }

    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);

    auto fb1Context = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb0Context = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb2Context = juce::dsp::ProcessContextReplacing<float>(fb2Block);

    LP1.process(fb0Context);
    AP2.process(fb0Context);

    HP1.process(fb1Context);
    filterBuffers[2] = filterBuffers[1];
    LP2.process(fb1Context);

    HP2.process(fb2Context);

}


void SimpleMBCAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateState();
    
    leftChannelFifo.update(buffer);
    rightChannelFifo.update(buffer);


    applyGain(buffer, inputGain);

    splitBands(buffer);

    for (size_t i = 0; i < filterBuffers.size(); ++i)
    {
        compressors[i].process(filterBuffers[i]);
    }

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear();

    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
        {
            for (auto channel = 0; channel < nc; ++channel)
            {
                inputBuffer.addFrom(channel, 0, source, channel, 0, ns);
            }
        };

    auto bandsAreSoloed = false;
    for (auto& comp : compressors)
    {
        if (comp.solo->get())
        {
            bandsAreSoloed = true;
            break;
        }
    }

    // If any band is soloed, add only the soloed bands to the output buffer
    // If none are soloed, add all non-muted bands to the output buffer
    if (bandsAreSoloed)
    {
        for (auto& comp : compressors)
        {
            if (comp.solo->get())
            {
                addFilterBand(buffer, filterBuffers[&comp - &compressors[0]]);
            }
        }
    }
    else
    {
        for (auto& comp : compressors)
        {
            if (!comp.mute->get())
            {
                addFilterBand(buffer, filterBuffers[&comp - &compressors[0]]);
            }
        }
    }

    applyGain(buffer, outputGain);
}


//==============================================================================
bool SimpleMBCAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SimpleMBCAudioProcessor::createEditor()
{
    //return new juce::GenericAudioProcessorEditor(*this);
    return new SimpleMBCAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleMBCAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);

}

void SimpleMBCAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
	{
		apvts.replaceState(tree);
	}
}


juce::AudioProcessorValueTreeState::ParameterLayout SimpleMBCAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    using namespace juce;
    using namespace Params;
    
    const auto& params = GetParams();
    
    auto gainRange = NormalisableRange<float>(-24.0f, 24.f, 0.5f, 1.f);

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Gain_In),
                                                     params.at(Names::Gain_In),
                                                     gainRange,
                                                     0));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Gain_Out),
        											 params.at(Names::Gain_Out),
        											 gainRange,
        											 0));


    auto thresholdRange = NormalisableRange<float>(MIN_THRESHOLD, MAX_DECIBELS, 1.f, 1.f);


    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_Low_Band),
                                                     params.at(Names::Threshold_Low_Band),
                                                     thresholdRange,
                                                     0));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_Mid_Band),
                                                     params.at(Names::Threshold_Mid_Band),
													 thresholdRange,
													 0));


    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_High_Band),
													 params.at(Names::Threshold_High_Band),
													 thresholdRange,
													 0));



    auto attackReleaseRange = NormalisableRange<float>(0.1f, 5000.f, 0.1f, 0.25f);

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Low_Band),
													 params.at(Names::Attack_Low_Band),
													 attackReleaseRange,
													 50.f));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Mid_Band),
													 params.at(Names::Attack_Mid_Band),
													 attackReleaseRange,
													 50.f));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_High_Band),
                                                     params.at(Names::Attack_High_Band),
													 attackReleaseRange,
													 50.f));



    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_Low_Band),
													 params.at(Names::Release_Low_Band),
													 attackReleaseRange,
													 50.f));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_Mid_Band),
													 params.at(Names::Release_Mid_Band),
													 attackReleaseRange,
													 50.f));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_High_Band),
													 params.at(Names::Release_High_Band),
													 attackReleaseRange,
													 50.f));


    auto choices = std::vector<double>{ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 15.0, 20.0, 50.0, 100.0, 150.0, 250.0 };
    juce::StringArray choiceStrings;
    for (auto choice : choices)
    {
        choiceStrings.add(juce::String(choice, 1));
    }


    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Low_Band),
													  params.at(Names::Ratio_Low_Band),
													  choiceStrings,
													  0));

    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Mid_Band),
													  params.at(Names::Ratio_Mid_Band),
													  choiceStrings,
													  0));

    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_High_Band),
                                                      params.at(Names::Ratio_High_Band),
													  choiceStrings,
													  0));


    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_Low_Band),
													params.at(Names::Bypassed_Low_Band),
													false));

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_Mid_Band),
													params.at(Names::Bypassed_Mid_Band),
													false));

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_High_Band),
                                                    params.at(Names::Bypassed_High_Band),
                                                    false));
    

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Mute_Low_Band),
                                                    params.at(Names::Mute_Low_Band),
                                                    false));

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Mute_Mid_Band),
                                                    params.at(Names::Mute_Mid_Band),
                                                    false));

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Mute_High_Band),
                                                    params.at(Names::Mute_High_Band),
                                                    false));


    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Solo_Low_Band),
                                                    params.at(Names::Solo_Low_Band),
                                                    false));

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Solo_Mid_Band),
                                                    params.at(Names::Solo_Mid_Band),
                                                    false));

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Solo_High_Band),
                                                    params.at(Names::Solo_High_Band),
                                                    false));



    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq),
													 params.at(Names::Low_Mid_Crossover_Freq),
													 NormalisableRange<float>(MIN_FREQUENCY, 999.f, 1.f, 1.f),
													 400.f));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq),
													 params.at(Names::Mid_High_Crossover_Freq),
													 NormalisableRange<float>(1000.f, MAX_FREQUENCY, 1.f, 1.f),
													 2000.f));






    return layout;
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleMBCAudioProcessor();
}
