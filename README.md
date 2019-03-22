# MAE_151: Music Box Project
## About

The goal of the MAE 151 project is to develop a music box capable of detecting/catching a ball and dropping it to the next music box at specified times. Additionally, when the ball is detected and caught, the music box must play a short series of notes, which are a measure from a song, and light up LEDs at the same time. 

## Update Log

**Update (3/22/19):** The most up-to-date program is now "No_High_Power_LED". I have found that the previous issue regarding the addressable LED strip may not be due to the library momentarilly turning off the interrupts. Rather, I believe that the main issue is due to ground looping. This is because I ran into the same issue when using PWM to dim a high power LED. Because I do not have time to fix this issue by the end of the project deadline, I ultimately decided to replace the high power LED with a 5 mm blue LED. However, if you are interested in resolving this issue, I believe that it can be solved by using a common mode choke or a star ground. A guarantee solution to this problem would be to supply the amplifier with its own source (separate from the audio source) so they do not share the sme ground. That said, this will increase the product cost. 

**Update (2/20/19):** The most up-to-date program is "Complete_Test_With_LEDs". Please note that the current issue (2/20/19) is that the LED's cause interference with MIDI communication between the Music Instrument Shield, causing a high pitch noise to play in the background. From online (i.e. https://forum.arduino.cc/index.php?topic=451853.0),  I have found that this is not an uncommon issue and is most likely because the Serial protocol (which MIDI data is sent over) requiring precise timing, but the neopixel (and FastLED) library turn off the hardware interrrupts when sending data to the LED strip. The hypothesis that the LED strip was interfering with the MIDI signals was proven to be the issue when only the LEDs were actuated and the speaker began playing noises based on the LED patterns from the Neopixel example code. One current solution I have come up with was replacing the addressable LED strip with a non-addressable LED strip or high power LEDs instead. Even if the addressable LED strip worked, the workarounds that will be required will most likely limit the Arduino too much for this project. 
