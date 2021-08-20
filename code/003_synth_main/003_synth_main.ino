#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <MIDI.h>
#include "VoiceManager.h"
#include "FXChain.h"
#include "waveshapeTable.h"

#define NUM_CONTROLS 48

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, interfaceMIDI); //for MIDI messages coming from interface controller (rx=0, tx=1)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, inputMIDI);     //for MIDI messages coming from external MIDI device through 5 pin din (rx=16, tx=17)

AudioInputI2S         ext_input;
AudioOutputI2S        i2s;

AudioControlSGTL5000  sgtl5000_1;

VoiceManager          vm;  //object for polyphonic voices/effects
FXChain               fx;  //object for a monophonic fx chain
AudioMixer4           outputMixer;
AudioSynthWaveformDc  masterLevel;
AudioEffectMultiply   masterMixer;

//chain voices->fx->output mixer
AudioConnection       patchCable0(vm.output, 0, fx.input, 0);
AudioConnection       patchCable1(ext_input, 0, fx.input, 1);
AudioConnection       patchCable2(ext_input, 0, outputMixer, 1);
AudioConnection       patchCable3(vm.output, 0, outputMixer, 2);
AudioConnection       patchCable4(fx.output, 0, outputMixer, 3);
AudioConnection       patchCable5(masterLevel, 0, masterMixer, 0); //using a dc to multiply instead of amp so we can ramp changes over time (no clicking)
AudioConnection       patchCable6(outputMixer, 0, masterMixer, 1);
AudioConnection       patchCable7(masterMixer, 0, i2s, 0);
AudioConnection       patchCable8(masterMixer, 0, i2s, 1);

byte controlValues[NUM_CONTROLS];

void setup() {
  AudioMemory(200);  //200 default (without kpss) //1000 ok //500 ok //200 ok
  Serial.begin(115200);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8); //set output headphone volume to a suitable default
  sgtl5000_1.dacVolume(1); //set dac output volume to a suitable default
  sgtl5000_1.lineOutLevel(13); //set line out to high level (range: 13 high to 31 low - values below 13 lead to clipping)
  sgtl5000_1.adcHighPassFilterDisable(); //this reduces some noise on the ext input

  outputMixer.gain(1, 0); //turn down line in mix by default (it's noisy)

  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleNoteOn(onNoteOn) ;
  usbMIDI.setHandleControlChange(onControlChange);
  usbMIDI.setHandleProgramChange(onProgramChange);

  interfaceMIDI.setHandleControlChange(onControlChange);
  interfaceMIDI.setHandleProgramChange(onProgramChange);
  interfaceMIDI.begin(MIDI_CHANNEL_OMNI);

  inputMIDI.setHandleNoteOn(onNoteOn);
  inputMIDI.setHandleNoteOff(onNoteOff);
  inputMIDI.setHandleControlChange(onControlChange);
  inputMIDI.begin(MIDI_CHANNEL_OMNI);

  setupSD();
}

int iter = 0;

void loop() {
  usbMIDI.read();
  interfaceMIDI.read();
  inputMIDI.read();
  vm.update(); //purely for arpeggiator

  if (iter++ % 1000000 == 0) {
    Serial.print(AudioProcessorUsage());
    Serial.print('\t');
    Serial.print(AudioProcessorUsageMax());
    Serial.print('\t');
    Serial.print(AudioMemoryUsage());
    Serial.print('\t');
    Serial.println(AudioMemoryUsageMax());
  }
}
