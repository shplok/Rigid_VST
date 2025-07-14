#pragma once

#include <JuceHeader.h>
class Rigid_testAudioProcessor;

class GestureEnabledEditor : public juce::Component,
                              private juce::Timer
{
public:
    GestureEnabledEditor(Rigid_testAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    Rigid_testAudioProcessor& processor;
    juce::String currentGesture;
    juce::Label gestureLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureEnabledEditor)
};

// GestureEnabledEditor.cpp
#include "GestureEnabledEditor.h"
#include "PluginProcessor.h"

GestureEnabledEditor::GestureEnabledEditor(Rigid_testAudioProcessor& p) : processor(p)
{
    gestureLabel.setText("Gesture: ", juce::dontSendNotification);
    gestureLabel.setFont(juce::Font(16.0f));
    addAndMakeVisible(gestureLabel);
    startTimerHz(10);
}

void GestureEnabledEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Current Gesture: " + currentGesture, getLocalBounds(), juce::Justification::centred, true);
}

void GestureEnabledEditor::resized()
{
    gestureLabel.setBounds(10, 10, getWidth() - 20, 40);
}

void GestureEnabledEditor::timerCallback()
{
    auto latest = processor.getLatestGesture();
    if (currentGesture != latest)
    {
        currentGesture = latest;
        repaint();
    }
}
