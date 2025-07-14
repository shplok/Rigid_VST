#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


class GestureEnabledEditor : public juce::Component, private juce::Timer
{
public:
    GestureEnabledEditor::GestureEnabledEditor(Rigid_testAudioProcessor& proc)
    : processor(proc)
{
    addAndMakeVisible(gestureLabel);
    gestureLabel.setFont(juce::Font(18.0f));
    gestureLabel.setText("Waiting for gesture...", juce::dontSendNotification);
    gestureLabel.setJustificationType(juce::Justification::centred);
    startTimerHz(10);
}

    void resized() override
    {
        gestureLabel.setBounds(getLocalBounds().reduced(10));
    }

private:
    void timerCallback() override
    {
        auto gesture = processor.getLatestGesture();
        if (gesture != lastGesture)
        {
            lastGesture = gesture;
            gestureLabel.setText("Gesture: " + gesture, juce::dontSendNotification);
        }
    }

    Rigid_testAudioProcessor& processor;
    juce::Label gestureLabel;
    juce::String lastGesture;
};
