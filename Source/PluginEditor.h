/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Rigid_testAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:

    Rigid_testAudioProcessorEditor (Rigid_testAudioProcessor&);
    ~Rigid_testAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::Label gainLabel;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Rigid_testAudioProcessor& audioProcessor;
    
    
    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Rigid_testAudioProcessorEditor)
};
