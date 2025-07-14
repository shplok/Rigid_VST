#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Rigid_testAudioProcessor::Rigid_testAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
        #if ! JucePlugin_IsMidiEffect
         #if ! JucePlugin_IsSynth
          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
         #endif
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
        #endif
      ),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

Rigid_testAudioProcessor::~Rigid_testAudioProcessor() = default;

const juce::String Rigid_testAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Rigid_testAudioProcessor::acceptsMidi() const      { return false; }
bool Rigid_testAudioProcessor::producesMidi() const     { return false; }
bool Rigid_testAudioProcessor::isMidiEffect() const     { return false; }
double Rigid_testAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int Rigid_testAudioProcessor::getNumPrograms()               { return 1; }
int Rigid_testAudioProcessor::getCurrentProgram()            { return 0; }
void Rigid_testAudioProcessor::setCurrentProgram (int)       {}
const juce::String Rigid_testAudioProcessor::getProgramName (int) { return {}; }
void Rigid_testAudioProcessor::changeProgramName (int, const juce::String&) {}

void Rigid_testAudioProcessor::prepareToPlay (double, int)
{
    if (!oscReceiver.connect(8000))
        DBG("Failed to bind to port 8000");
    else
        oscReceiver.addListener(this, "/gesture");  // Listen only to /gesture messages
}

void Rigid_testAudioProcessor::oscMessageReceived(const juce::OSCMessage& message)
{
    if (message.size() == 1 && message[0].isString())
    {
        juce::String gesture = message[0].getString();
        DBG("Received gesture: " + gesture);

        lastGesture = gesture;

        if (gesture == "fist")
            parameters.getParameter("gain")->setValueNotifyingHost(0.0f);
        else if (gesture == "palm")
            parameters.getParameter("gain")->setValueNotifyingHost(1.0f);
        else if (gesture == "point")
            parameters.getParameter("gain")->setValueNotifyingHost(0.7f);
        else if (gesture == "victory")
            parameters.getParameter("gain")->setValueNotifyingHost(0.3f);
    }
}

void Rigid_testAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Rigid_testAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
}
#endif

void Rigid_testAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float gainValue = parameters.getRawParameterValue("gain")->load();

    if (lastGesture == "palm") {
        // Granular effect example when open palm is detected
        int grainSize = std::min(200, buffer.getNumSamples());

        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            float* data = buffer.getWritePointer(channel);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                data[i] = data[i % grainSize] * gainValue;
        }
    } else {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
            buffer.applyGain(channel, 0, buffer.getNumSamples(), gainValue);
    }
}

bool Rigid_testAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* Rigid_testAudioProcessor::createEditor()
{
    return new Rigid_testAudioProcessorEditor (*this);
}

void Rigid_testAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Rigid_testAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout Rigid_testAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "gain", "Gain", 0.0f, 1.0f, 0.5f
    ));

    return { params.begin(), params.end() };
}

juce::String Rigid_testAudioProcessor::getLatestGesture() const
{
    return lastGesture;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Rigid_testAudioProcessor();
}
