#pragma once

#include <JuceHeader.h>
#include <juce_osc/juce_osc.h>

class Rigid_testAudioProcessor : public juce::AudioProcessor,
                                  private juce::OSCReceiver,
                                  private juce::OSCReceiver::ListenerWithOSCAddress<juce::OSCReceiver::MessageLoopCallback>
{
public:
    Rigid_testAudioProcessor();
    ~Rigid_testAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::String getLatestGesture() const;

private:
    // This override handles OSC messages with the address filter
    void oscMessageReceived(const juce::OSCMessage& message) override;

    juce::OSCReceiver oscReceiver;
    juce::String lastGesture;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Rigid_testAudioProcessor)
};
