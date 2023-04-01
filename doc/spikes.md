# Spikes that hurt - How to dejam the DC motors

**Edit:** As a very positive side effect of switching to Comunello AIRWIN actuators this problem just disappeared. The actuators are properly de-jammed and don't cause any trouble. Also the stability of the whole setup improved remarkably.

~~Then everything was in place: the actuators were mounted at the windows, the sensors distributed in the rooms and outside, an initial version of the software was ready. But after operating the actuators for some seconds, the Feather was suddenly reset and restarted the program.~~

~~My first thought was that interference caused by the motors would occur here. And I was right about that. The actuators have no interference suppression. The manufacturer has probably saved this money in order to achieve the favourable price. Fortunately, there is enough space in the junction box. ~~

~~Typically you can do two things to cope with interferences:~~

* ~~You can try to suppress them where they originate~~
* ~~You can try to filter them where they affect your circuit~~

~~Whenever possible, the first approach is better because it attacks the causes. Additionally, filters at the inputs are also helpful. I connected a capacitor with 100nF and a resistor with 10Ohm in series and connected it directly to the motor terminals. One end was soldered to the lower limit switch and the other was inserted into the upper terminal. The series resistor limits the inrush current when switching the motor on.~~

~~(insert an image here)~~

~~Additionally I added more 100nF capacitors at the following places:~~

* ~~between the terminals of the 36V power supply~~
* ~~between the terminals of the 5V power supply~~
* ~~between the +5V and GND pins of the sensors~~

~~Not sure whether this is really necessary, but just to be on the safe side...~~
