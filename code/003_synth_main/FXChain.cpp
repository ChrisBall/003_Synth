#include "Arduino.h"
#include "FXChain.h"

FXChain::FXChain() {
  //set default values for FXChain here 
  granular1.begin(grains, GRANULARLENGTH);
  bitCrusher1.bits(16); //this needs initialising here before it will pass audio
  //tapeDelayFilter1.begin(filterCoeffs[127],8);
  waveshaper1.shape(waveShapes[0], WAVESHAPE_SIZE);
  input.gain(1, 0); //turn off line in by default

  tremoloLFO.offset(0.5);
  tremoloLFO.begin(0.5, 5, WAVEFORM_SINE);

  waveshaper1Gain.gain(1);

  tapeDelayFilter1.setLowpass(0, 400, 0.707);
  tapeDelayMixer1.gain(1, 0);
  tapeBypassMix.gain(0);  //turn off tape bypass by default
  tapeInMix.gain(1);  //turn off tape bypass by default

  waveshaper2Gain.gain(1);

}
