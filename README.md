# 003_Synth
Code &amp; files for making my audio-lib based synth; 003

Based on a teensy 4.0 with assoc. audio board.

You can use this code without building a controller - the synth can be controlled via MIDI. There is a processing program in the code folder to facilitate this.

To get it making a sound, there's some extra work:

-You'll need to set the master volume (CC44, channel 3 - the device only responds to controller values on channel 3). 
-To get sound through the effects chain, set the two distortion gains (CC25, CC41 - channel 3) which default to 0.
-To save a preset (SD card required) send program change on channel 16. To load: program change on channel 3. Presets are saved as plain text in folder "/PRESETS/" on the SD.
