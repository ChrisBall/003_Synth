# 003_Synth
Code &amp; files for making my audio-lib based synth; 003

Based on a teensy 4.0 with assoc. audio board.

The synth is actually 2 devices - a teensy 4.0 with audio board and a separate "nano every"-based midi controller, with 48 pots, a rotary encoder and OLED display. 



Description of the contents of the code folder:

-"003_synth_main" contains the main arduino sketch.

-"Processing_003_controller_board" contains a basic processing sketch for controlling the synth using [the midibus](https://github.com/sparks/themidibus) library.

-"ctrlr interface" contains a similar midi interface for the [ctrlr software](https://github.com/RomanKubiak/ctrlr).

-"modified audio lib files" contains a modified delay effect for tape simulation. You'll need to replace the originals in the teensy audio lib folder.

-"nano_controller" contains the code that runs on the HMI controller.



You can use this code without building a controller - the synth can be controlled via MIDI. There is a processing program or ctrlr interface in the code folder to facilitate this.

Compilation notes:

-To compile my digital "tape" delay module you'll need to replace the teensy audio lib files (effect_delay.cpp and effect_delay.h) with my versions in the "modified audio lib files" folder.
-Build with "smallest code" option

To get it making a sound, there's some extra work:

-You'll need to set the master volume (CC44, channel 3 - the device only responds to controller values on channel 3). 
-To get sound through the effects chain, set the two distortion gains (CC25, CC41 - channel 3) which default to 0.
-To save a preset (SD card required) send program change on channel 16. To load: program change on channel 3. Presets are saved as plain text in folder "/PRESETS/" on the SD.
