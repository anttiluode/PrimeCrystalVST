#pragma once
#include <JuceHeader.h>
#include <atomic>

class PrimeCrystalProcessor : public juce::AudioProcessor
{
public:
    PrimeCrystalProcessor();
    ~PrimeCrystalProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // --- GUI hooks ---
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    
    // --- Boilerplate ---
    const juce::String getName() const override { return "Prime Crystal"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    // --- Expose variables for the Visualizer ---
    double getSimTime() const { return t_sim.load(); }
    float getSpeed() const { return speedParam ? speedParam->load() : 0.5f; }
    int getPrimeCount() const { return NUM_PRIMES; }
    float getPrime(int index) const { return primes[index]; }

    // --- Parameter Management ---
    juce::AudioProcessorValueTreeState apvts;

private:
    float mexicanHatDistortion (float x, float drive);
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioBuffer<float> delayBuffer;
    int writePosition { 0 };
    double currentSampleRate { 44100.0 };
    
    std::atomic<double> t_sim { 0.0 };
    std::atomic<float>* speedParam = nullptr;
    std::atomic<float>* driveParam = nullptr;

    static constexpr int NUM_PRIMES = 6;
    const float primes[NUM_PRIMES] = { 2.0f, 3.0f, 5.0f, 7.0f, 11.0f, 13.0f };
    float ln_p[NUM_PRIMES];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PrimeCrystalProcessor)
};