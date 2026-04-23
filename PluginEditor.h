#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

// Custom Component that draws the rotating Prime Arms
class DirichletSpiral : public juce::Component, public juce::Timer
{
public:
    DirichletSpiral(PrimeCrystalProcessor& p) : processor(p) 
    { 
        startTimerHz(30); // 30 FPS animation
    }
    
    void paint(juce::Graphics& g) override;
    void timerCallback() override { repaint(); }

private:
    PrimeCrystalProcessor& processor;
};

class PrimeCrystalEditor : public juce::AudioProcessorEditor
{
public:
    PrimeCrystalEditor (PrimeCrystalProcessor&);
    ~PrimeCrystalEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PrimeCrystalProcessor& audioProcessor;

    juce::Slider speedSlider;
    juce::Slider driveSlider;
    juce::Label speedLabel;
    juce::Label driveLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

    DirichletSpiral visualizer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PrimeCrystalEditor)
};