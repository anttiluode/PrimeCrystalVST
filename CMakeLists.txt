cmake_minimum_required(VERSION 3.22) # JUCE requires CMake 3.22 or higher
project(PrimeCrystal VERSION 1.0.0)

# Bring the JUCE framework into our build
add_subdirectory(JUCE)

# Define our plugin target
juce_add_plugin(PrimeCrystal
    PRODUCT_NAME "Prime Crystal"          #
    VERSION 1.0.0                         #
    PLUGIN_MANUFACTURER_CODE "PRME"       #
    PLUGIN_CODE "Crys"                    #
    FORMATS VST3                          #
    IS_SYNTH FALSE                        #
    NEEDS_MIDI_INPUT FALSE                #
    NEEDS_MIDI_OUTPUT FALSE               #
    COPY_PLUGIN_AFTER_BUILD TRUE)         # Automatically installs to your VST3 folder

# Add our source files
target_sources(PrimeCrystal PRIVATE
    PluginProcessor.cpp)

# Link the required JUCE modules
target_link_libraries(PrimeCrystal PRIVATE
    juce::juce_audio_processors
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)

target_compile_definitions(PrimeCrystal PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_BUILD_VST2=0
)

# Add our source files
target_sources(PrimeCrystal PRIVATE
    PluginProcessor.cpp
    PluginEditor.cpp)

# Generate the JuceHeader.h so our C++ files can find it
juce_generate_juce_header(PrimeCrystal)