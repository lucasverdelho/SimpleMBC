# SimpleMBC: A Fundamental Multiband Compressor Implementation Using the JUCE Framework

## Introduction

This project presents a straightforward realization of a multiband compressor utilizing the JUCE framework. The primary objective of this endeavor was to delve into a more intricate and interconnected project design, encompassing both enhanced functionality and an intuitive graphical user interface (GUI). As my second project employing JUCE, I have observed significant advancements in my comprehension of the framework, reflected in the refined final outcome.

## Features

- A three-band compressor with threshold, ratio and attack
- Global Gain Controls
- A Spectrum Analyzer
- Bypass, Solo, and Mute Buttons for each band
- Customizable crossover frequencies
- Simple and clean design

## Design

The design of this plugin is influenced by the Dark theme Design language of Ableton Live, featuring only two colors and various shades of gray. Adhering to Ableton's design language has been a gratifying learning experience, aligning seamlessly with the plugin's purpose by embodying a simple and clean design aesthetic. Areas for potential enhancement in the future include refining the font of the buttons and adjusting the curvature of the spectrum analyzer graph.

![First Version Prototype](https://github.com/lucasverdelho/SimpleMBC/blob/main/Img/Version3.png)

# TODO

- [x] Fix Control Bar Appearance
- [x] Tune Colours
- [x] Make Spectrum Analyzer Background a much darker gray in order to improve contrast
- [x] Fix Control Bar Appearance
- [x] Change the Power Button to Abletons style (an orange circle with a thin black outline)
- [ ] Search for a better font for the buttons
- [ ] Make the font bold in the band select buttons as well as their respective bypass, solo and mute buttons
- [ ] Make the Spectrum Analyzer Graph rounder, maybe using bezier curves
- [ ] Add a Short decay time to the spectrum analyzer, leaving a trail of the previous peaks