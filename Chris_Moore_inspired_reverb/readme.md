## Chris Moor inspired FDN in MAX/MSP using Multi Channel expansion

Es wurden primär die Werte für die Delaytimes, aus der Implementation von Tom Erbe übernommen. 
Die Idee ist es inspiriert von Geraint Luff den Schaltplan in der Multi Channel expansion von Max umzusetzen.
Der Vorteil ist, dass dadruch das Patch komprimierter wird und der funktionsumfang teilweise erweitert wird.
z.B. kann man in einem Multichannel Delay alle delaytimes durch eine harmoniche Riehe modulieren oder wie in meiner Umsetztung Zufällig in gewissen Grenzen verteilen. 
Der große Nachteil an diesem Ansatz ist, dass es keine Hadamard Matrix gibt, welche sehr wichtig ist. 
Es gibt jedoch eine Multichannel Mix Matrix, über eine Liste mit Werten beschrieben werden kann. 
Das wurde gemacht. Leider scheint das nur teilweise geklappt zu haben, 
denn im Vergleich zur Tom Erbe Chris Moor Implementation erreicht die Multi Channel implementation nicht den gleichen Grad an Diffusion im FDN. 
Warum genau das der Fall ist, ist leider nicht offensichtlich. 
Nach diesem Projekt würde Ich aber davon abraten einen Multichannel Reverb in Max zu schreiben, 
sondern sich eher an den Singel Channel Implementationen von Tom Erbe zu orientieren. Nichts desto trotz ist der entstandene Reverb
, der sich ehr wie ein Cluster Delay Chorus anhört, auch manchem Audiomaterial wohlklingend.
 


Sources : 

http://tre.ucsd.edu/wordpress/?p=696


Tom Erbe/Soundhack "Designing the Make Noise Erbe-Verb" Reverb Design Lecture (Remastered) 
https://www.youtube.com/watch?v=Il_qdtQKnqk vom 2024.06.17


Geraint Luff - Let's Write a Reverb - ADC21 https://youtu.be/6ZK2Goiyotk?si=ZKnUK5h3LWqT8w4v 
