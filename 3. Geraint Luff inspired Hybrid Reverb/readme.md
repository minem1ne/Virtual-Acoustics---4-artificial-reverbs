This reverb is based on a Design by Geraint Luff presented at ADC 2021.

It uses feedback delay networks to create a diffuse and long lasting reverb.
For early reflections a convolution with a shortened room IR has been implemented.
Additionally frequency dependent damping is simulated using 3 shelf filter with decreasing gain
towards higher frequencies.

The provided codebase wraps the reverb design code by Geraint Luff (diffusior class and feedback class) into a real time
vst plugin. For that the JUCE framework is used. The convolution class uses a partitioned convolution algorithm.
Parameter control over the early reflections mix, dry mix, wet mix and reverb decay was implemented. 

The room impulse response used belongs to:

Stewart, Rebecca and Sandler, Mark. "Database of Omnidirectional and B-Format Impulse Responses", 
in Proc. of IEEE Int. Conf. on Acoustics, Speech, and Signal Processing (ICASSP 2010), Dallas, Texas, March 2010

Geraint Luff base code:

https://github.com/Signalsmith-Audio/reverb-example-code

The reverb code is located in the reverb folder, the dsp folder contains some helper classes and the juce code are
the pluginEditor and pluginProcessor files









