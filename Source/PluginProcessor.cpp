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

    floatHelper(compressor.threshold, Names::Threshold_Low_Band);
    floatHelper(compressor.attack, Names::Attack_Low_Band);
    floatHelper(compressor.release, Names::Release_Low_Band);

    auto choiceHelper = [&apvts = this ->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    choiceHelper(compressor.ratio, Names::Ratio_Low_Band);

    auto boolHelper = [&apvts = this ->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };

    boolHelper(compressor.bypassed, Names::Bypassed_Low_Band);


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

    compressor.prepare(spec);



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


    compressor.updateCompressorSettings();
    compressor.process(buffer);


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


    auto attackReleaseRange = NormalisableRange<float>(0.1f, 5000.f, 0.1f, 0.25f);

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Low_Band),
													 params.at(Names::Attack_Low_Band),
													 attackReleaseRange,
													 50.f));

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_Low_Band),
													 params.at(Names::Release_Low_Band),
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

    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_Low_Band),
													params.at(Names::Bypassed_Low_Band),
													false));


    return layout;
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleMBCAudioProcessor();
}
