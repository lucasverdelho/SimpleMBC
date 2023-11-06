/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"












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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

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


    for (auto& buffer : filterBuffers)
	{
		buffer.setSize(spec.numChannels, samplesPerBlock);
	}
}















void SimpleMBCAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleMBCAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif





























void SimpleMBCAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (auto& comp : compressors)
		comp.updateCompressorSettings();

    //compressor.updateCompressorSettings();
    //compressor.process(buffer);

    for (auto& fb : filterBuffers)
    {
         fb = buffer;
	}
        
    auto lowMidCutoff = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoff);
    HP1.setCutoffFrequency(lowMidCutoff);
    
    auto midHighCutoffFreq = midHighCrossover->get();
    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);

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



    // If any of the band is soloed, then only add the soloed bands to the output buffer
    // else if none of the bands are soloed, then add all the non muted bands to the output buffer

    auto bandsAreSoloed = false;
    for (auto& comp : compressors)
    {
        if (comp.solo->get())
        {
            bandsAreSoloed = true;
            break;
        }
    }

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

 /*   addFilterBand(buffer, filterBuffers[0]);
    addFilterBand(buffer, filterBuffers[1]);
    addFilterBand(buffer, filterBuffers[2]);*/






}
























//==============================================================================
bool SimpleMBCAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleMBCAudioProcessor::createEditor()
{
    //return new SimpleMBCAudioProcessorEditor (*this);

    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleMBCAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);

}

void SimpleMBCAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

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
    

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_Low_Band),
                                                     params.at(Names::Threshold_Low_Band),
                                                     NormalisableRange<float>(-60.0f, 12.f, 1.f, 1.f),
                                                     0));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_Mid_Band),
                                                     params.at(Names::Threshold_Mid_Band),
													 NormalisableRange<float>(-60.0f, 12.f, 1.f, 1.f),
													 0));


    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_High_Band),
													 params.at(Names::Threshold_High_Band),
													 NormalisableRange<float>(-60.0f, 12.f, 1.f, 1.f),
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
													 NormalisableRange<float>(20.f, 999.f, 1.f, 1.f),
													 400.f));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq),
													 params.at(Names::Mid_High_Crossover_Freq),
													 NormalisableRange<float>(1000.f, 20000.f, 1.f, 1.f),
													 2000.f));






    return layout;
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleMBCAudioProcessor();
}
