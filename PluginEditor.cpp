#include "PluginProcessor.h"
#include "PluginEditor.h"

// --- The Riemann Arm Visualizer ---
void DirichletSpiral::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float cx = bounds.getWidth() * 0.5f;
    float cy = bounds.getHeight() * 0.5f;
    
    double t = processor.getSimTime();
    float f0 = processor.getSpeed();
    
    float x = cx;
    float y = cy;
    float scale = std::min(bounds.getWidth(), bounds.getHeight()) * 0.15f;

    // Draw each prime arm
    for (int i = 0; i < processor.getPrimeCount(); ++i)
    {
        float p = processor.getPrime(i);
        float phase = f0 * std::log(p) * (float)t;
        
        float dx = std::cos(phase) * scale;
        float dy = std::sin(phase) * scale;
        
        // Color mapping similar to your Python script
        float hue = 0.05f + 0.9f * ((float)i / processor.getPrimeCount());
        g.setColour(juce::Colour::fromHSV(hue, 0.8f, 0.9f, 1.0f));
        
        g.drawLine(x, y, x + dx, y + dy, 2.0f);
        g.fillEllipse(x + dx - 2.5f, y + dy - 2.5f, 5.0f, 5.0f);
        
        x += dx;
        y += dy;
        
        // Taper the length of higher primes so they spiral nicely
        scale *= 0.85f; 
    }
}

// --- The Main UI Editor ---
PrimeCrystalEditor::PrimeCrystalEditor (PrimeCrystalProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), visualizer(p)
{
    // Configure Speed Slider
    speedSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    speedLabel.setText("Prime Speed", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(speedLabel);

    // Configure Drive Slider
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    driveLabel.setText("Hat Drive", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(driveLabel);

    // Attach sliders to audio parameters
    speedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SPEED", speedSlider);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);

    addAndMakeVisible(visualizer);

    setSize (600, 400);
}

void PrimeCrystalEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(5, 7, 10)); // Dark void background
    g.setColour(juce::Colours::cyan);
    g.setFont(20.0f);
    g.drawText("Prime-Driven Domain Walls", getLocalBounds().removeFromTop(40), juce::Justification::centred, true);
}

void PrimeCrystalEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(40); // Leave space for title

    // Place the visualizer on the right side
    visualizer.setBounds(area.removeFromRight(400));

    // Place sliders on the left side
    int sliderSize = 100;
    auto leftArea = area;
    
    speedSlider.setBounds(leftArea.removeFromTop(sliderSize).withSizeKeepingCentre(sliderSize, sliderSize));
    speedLabel.setBounds(speedSlider.getBounds().translated(0, 50));
    
    leftArea.removeFromTop(20); // Spacing
    
    driveSlider.setBounds(leftArea.removeFromTop(sliderSize).withSizeKeepingCentre(sliderSize, sliderSize));
    driveLabel.setBounds(driveSlider.getBounds().translated(0, 50));
}