DFDN reverb 
author: Jonas Schenkyr

adjustable algorithmic reverb effect
works in stereo at 48kHz sample rate

adjustable parameters:

predelay: predelay time, adjustable from 0 to 500 ms
length: reverberation length adjustable from a few milliseconds up to 30 seconds
size: scales the delaytimes to simulate different room sizes
lowcut frequency: frequency of the lowcut filter, adjustable from 10 Hz to 6.3 kHz
highshelf frequency: frequency of the highshelf filter, adjustable from 450 Hz to 20 kHz
highshelf gain: gain of the highshelf filter, from -80 dB to +20 dB
xfeed: adjustable crossfeed of the left and right input signals to the right and left reverberation networks
stereo width: stereo widening effect, from 0 to 30 ms adjustable delay applied to the right channel
dry: gain of the dry input signal
wet: gain of the reverberation


the reverb effect is produced by two feedback delay networks, one for each channel. the feedback delay networks each consist of 16 delay lines with feedback through a 16x16 feedback matrix. The matrix contains the feedback gain values which are optimized to achieve a "colorless" sound (see https://www.dafx.de/paper-archive/2023/DAFx23_paper_32.pdf DIFFERENTIABLE FEEDBACK DELAY NETWORK FOR COLORLESS REVERBERATION Gloria Dal Santo, Karolina Prawda, Sebastian J. Schlecht˚ , and Vesa Välimäki) using the fdn toolbox by Sebastian Schlecht https://www.sebastianjiroschlecht.com/project/fdntb/).

implemented in Faust (https://faustide.grame.fr/)

