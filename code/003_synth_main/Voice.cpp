#include "Arduino.h"
#include "Voice.h"

Voice::Voice() {
  dc1.amplitude(0);

  envelope1.attack(10);
  envelope1.sustain(1);
  envelope1.release(1000);
  envelope2.attack(10);
  envelope2.sustain(1);
  envelope2.release(1000);

  dc2.amplitude(1.0);
  filter1.frequency(27.5);
  filter1.resonance(0.95);
  filter1.octaveControl(6);

  waveform1.begin(0, REF_FREQ, WAVEFORM_SINE);
  waveform2.begin(0, REF_FREQ, WAVEFORM_SINE);
}

void Voice::noteOn(byte MIDINOTE, byte MIDIVELOCITY, float GLIDETIME, boolean CONT) {
  //instead of directly setting frequency we set a fade value on a dc module that feeds into the waveformMod module

  float ff = float(MIDINOTE - REF_NOTE) / 96; //96=12 (notes per octave) * 8 (default control octaves)
  float v = (float)MIDIVELOCITY * 0.007874015;

  noise1.amplitude(v);

  if (getState()) {
    dc1.amplitude(ff, GLIDETIME);
  } else {
    if(CONT){
      dc1.amplitude(ff, GLIDETIME);
    }else{
      dc1.amplitude(ff, 0);
    }
    waveform1.amplitude(v);
    waveform2.amplitude(v);
    envelope1.noteOn();
    envelope2.noteOn();
    dc2.amplitude(v);  //velocity affects max cutoff frequency available from the filter envelope
  }

  note = MIDINOTE;
  state = true;
}

void Voice::noteOff(byte MIDINOTE, byte MIDIVELOCITY) {
  envelope1.noteOff();
  envelope2.noteOff();
  releaseAge = millis();
  state = false;
  note = 255;
}
