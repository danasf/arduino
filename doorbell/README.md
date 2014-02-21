Arduino-Based Doorbell
======

Arduino-based doorbell, running on an EtherTen (http://www.freetronics.com/products/etherten), outputs sound and light. 

door2.ino - sample doorbell sketch 

pitches.h - frequency to notes 

ring.py - python cgi to log doorbell buzz (or do something else)

Pin Assignments
-----

Buzzer signal (via opto-isolator) - 5
LED strip - 7
Speaker - 8
Ethernet/SD - 4,10,11,12,13

To Do
-----

* [ ] mount in a stable enclosure
* [x] move transistor (sound) from pin 12 to another pin so we can use ethernet. (4,10,11,12,13 are used by ethernet/sd card. 5 is buzzer signal, 7 is LEDs)
* [ ] run an ethernet cable to the doorbell
* [ ] Improve and expand doorbell code!


Troubleshooting 
-----

* Check your connections. Is a speaker connected to the green terminals of the arduino, an audio cable is plugged into the intercom box by the top door?
* Hit the reset key on the Arduino
* Ring the door bell. Does it work?
* Re-upload or modify the Arduino sketch


More info
-----

https://noisebridge.net/wiki/Doorbell
