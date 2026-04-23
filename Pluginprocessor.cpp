#include "PluginProcessor.h"
#include "PluginEditor.h"

PrimeCrystalProcessor::PrimeCrystalProcessor() 
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, juce::Identifier("PrimeCrystal"), createParameterLayout())
{
    speedParam = apvts.getRawParameterValue("SPEED");
    driveParam = apvts.getRawParameterValue("DRIVE");
}

juce::AudioProcessorValueTreeState::ParameterLayout PrimeCrystalProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    // Speed slider: 0.01 to 2.0
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SPEED", "Prime Speed", 0.01f, 2.0f, 0.5f));
    // Mexican Hat Drive: 0.1 to 3.0
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Hat Drive", 0.1f, 3.0f, 1.2f));
    return { params.begin(), params.end() };
}

void PrimeCrystalProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    currentSampleRate = sampleRate;
    delayBuffer.setSize (getTotalNumOutputChannels(), (int)(2.0 * sampleRate));
    delayBuffer.clear();
    writePosition = 0;
    t_sim.store(0.0);

    for (int i = 0; i < NUM_PRIMES; ++i)
        ln_p[i] = std::log(primes[i]);
}

float PrimeCrystalProcessor::mexicanHatDistortion (float x, float drive)
{
    x *= drive;
    float out = 1.5f * x - 0.5f * (x * x * x);
    return juce::jlimit (-1.0f, 1.0f, out);
}

void PrimeCrystalProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused (midiMessages);

    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const int delayBufferSize = delayBuffer.getNumSamples();
    auto* channelDataL = buffer.getWritePointer (0);
    auto* channelDataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;
    auto* delayDataL = delayBuffer.getWritePointer (0);
    auto* delayDataR = delayBuffer.getNumChannels() > 1 ? delayBuffer.getWritePointer (1) : delayDataL;

    // Grab live UI values
    float f_0 = speedParam->load();
    float currentDrive = driveParam->load();
    double local_t = t_sim.load();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float inL = channelDataL[sample];
        float inR = channelDataR != nullptr ? channelDataR[sample] : inL;

        float tapSumL = 0.0f;
        float tapSumR = 0.0f;

        for (int p_idx = 0; p_idx < NUM_PRIMES; ++p_idx)
        {
            float lfoVal = std::cos(f_0 * ln_p[p_idx] * (float)local_t);
            int delayFrames = static_cast<int>((primes[p_idx] / 20.0f) * currentSampleRate);
            
            int readPosition = writePosition - delayFrames;
            if (readPosition < 0) readPosition += delayBufferSize;

            tapSumL += delayDataL[readPosition] * (0.5f + 0.5f * lfoVal);
            tapSumR += delayDataR[readPosition] * (0.5f - 0.5f * lfoVal);
        }

        tapSumL /= (float)NUM_PRIMES;
        tapSumR /= (float)NUM_PRIMES;

        float feedbackL = mexicanHatDistortion (tapSumL, currentDrive);
        float feedbackR = mexicanHatDistortion (tapSumR, currentDrive);

        float feedbackAmount = 0.7f;
        delayDataL[writePosition] = inL + feedbackL * feedbackAmount;
        delayDataR[writePosition] = inR + feedbackR * feedbackAmount;

        channelDataL[sample] = inL * 0.5f + feedbackL * 0.8f;
        if (channelDataR != nullptr)
            channelDataR[sample] = inR * 0.5f + feedbackR * 0.8f;

        writePosition = (writePosition + 1) % delayBufferSize;
        local_t += 1.0 / currentSampleRate;
    }
    
    t_sim.store(local_t);
}

juce::AudioProcessorEditor* PrimeCrystalProcessor::createEditor()
{
    return new PrimeCrystalEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PrimeCrystalProcessor();
}