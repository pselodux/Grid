# Teensy Grid

### Description
8x8 grid controller using Teensy and Arduinome PCB. Currently _almost_ finished. I'll add schematics once it stops being a breadboard project :)

If you see this code and have a better idea about how to implement varying brightness levels on an 8x8 LED matrix, let me know.

### Features so far
* Incoming MIDI notes (0–63) light each pad, with two brightness levels depending on velocity (0–63 = dim, 64–127 = bright)
* Each pad sends a MIDI note (same as the ones required to light the pads) at 127 velocity.
* Decoupled, ie. pads don't light up when pressed unless there is some code/MIDI feedback in between output and input.

### Possible future features
* Possible built-in sequencer mode, for Norns/computer-free operation
* 3.5mm MIDI out, so a USB host isn't required
