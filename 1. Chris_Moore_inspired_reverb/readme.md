## Chris Moor inspired FDN in MAX/MSP using Multi Channel expansion

The values for the delay times were primarily taken from Tom Erbe's implementation. 
The idea is inspired by Geraint Luff to implement the circuit diagram in the Multi Channel expansion of Max.
The advantage of this is that the patch is more compressed and the range of functions is partially extended.
For example, in a multichannel delay you can modulate all delay times through a harmonic series or, as in my implementation, distribute them randomly within certain limits. 
The big disadvantage of this approach is that there is no Hadamard matrix, which is very important. 
However, there is a multichannel mix matrix that can be described using a list of values. 
This was done. Unfortunately, it seems to have worked only partially, 
because compared to the Tom Erbe Chris Moor implementation, the Multi Channel implementation does not achieve the same degree of diffusion in the FDN. 
Unfortunately, it is not clear exactly why this is the case. 
After this project, however, I would advise against writing a multichannel reverb in Max, 
but rather to follow the single channel implementations of Tom Erbe. Nevertheless, the resulting reverb
which sounds more like a cluster delay chorus, also sounds good with some audio material. 


Sources : 

http://tre.ucsd.edu/wordpress/?p=696


Tom Erbe/Soundhack "Designing the Make Noise Erbe-Verb" Reverb Design Lecture (Remastered) 
https://www.youtube.com/watch?v=Il_qdtQKnqk vom 2024.06.17


Geraint Luff - Let's Write a Reverb - ADC21 https://youtu.be/6ZK2Goiyotk?si=ZKnUK5h3LWqT8w4v 
