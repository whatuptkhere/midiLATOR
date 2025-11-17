# midiLATOR
Calculator becomes MIDI controller. 

This is a simple project to turn a calculator (or other device with a grid-read keypad) into a WiFi MIDI controller. 

Based around the ESP32, it uses the AppleMIDI library. The device presents itself as a WiFi access point. By joining the AP and running RTPMIDI software, the MIDI data can be routed to the audio software of your choice.
