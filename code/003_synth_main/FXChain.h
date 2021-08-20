#ifndef FXChain_h
#define FXChain_h
#include "Arduino.h"
#include <Audio.h>
#include "waveshapeTable.h"
#include "effect_platervbstereo.h"

#define GRANULARLENGTH 12800

class FXChain {
  public:
    FXChain();
    
    AudioMixer4               input;

    //tremolo
    AudioSynthWaveformModulated tremoloLFO;     //set tremolo frequency + amplitude in this object
    AudioEffectMultiply       tremoloMult;

    //waveshaper + limiting
    AudioEffectWaveshaper     waveshaper1;
    AudioAmplifier            waveshaper1Gain;

    //bitcrusher
    AudioEffectBitcrusher     bitCrusher1;

    //granular
    AudioEffectGranular       granular1;  
    AudioMixer4               granularMixer1;

    //ring mod
    AudioSynthWaveformSine    ringSine;
    AudioEffectMultiply       ringMult;
    AudioMixer4               ringMixer;

    //tape delay simulator
    AudioEffectTapeDelay      tapeDelay1;
    AudioFilterBiquad         tapeDelayFilter1; //change to fir?
    AudioAmplifier            tapeInMix;
    AudioAmplifier            tapeBypassMix;
    AudioMixer4               tapeDelayMixer1;
    AudioMixer4               tapeDelayMixer2;

    //filter
    AudioFilterStateVariable  filter;
    AudioMixer4               filterTypeMixer;
    AudioMixer4               filterMixer;

    //waveshaper + limiting (part 2)
    AudioEffectWaveshaper     waveshaper2;
    AudioAmplifier            waveshaper2Gain;

    //reverb
    AudioEffectPlateReverb    reverb;
    AudioMixer4               reverbMixer;
    
    //output
    AudioAmplifier            output;
    
    AudioConnection patchCord[32] = {
      {input,0,           tremoloMult,0},
      {tremoloLFO,0,      tremoloMult,1},
      {tremoloMult,0,     waveshaper1,0},
      {waveshaper1,0,     waveshaper1Gain,0},
      {waveshaper1Gain,0, bitCrusher1,0},    
      {bitCrusher1,       granular1},         
      {bitCrusher1,0,     granularMixer1,0},  
      {granular1,0,       granularMixer1,1},
      {granularMixer1,0,  ringMult,0},
      {ringSine,0,        ringMult,1},
      {ringMult,0,        ringMixer,0},  
      {granularMixer1,0,  ringMixer,1},
      {ringMixer,0,       tapeInMix,0},
      {ringMixer,0,       tapeBypassMix,0},
      {tapeInMix,0,       tapeDelayMixer1,0},
      {tapeDelayMixer1,   tapeDelay1},
      {tapeDelay1,        tapeDelayFilter1},
      {tapeDelayFilter1,0,tapeDelayMixer1,1},
      {tapeDelayMixer1,0, tapeDelayMixer2,0},
      {tapeBypassMix,0,   tapeDelayMixer2,1},
      {tapeDelayMixer2,0, filter,0},
      {tapeDelayMixer2,0, filterMixer,0},
      {filter,0,          filterTypeMixer,0},
      {filter,1,          filterTypeMixer,1},
      {filter,2,          filterTypeMixer,2},
      {filterTypeMixer,0, filterMixer,1},
      {filterMixer,0,     waveshaper2,0},
      {waveshaper2,0,     waveshaper2Gain,0},
      {waveshaper2Gain,0, reverb,0},
      {waveshaper2Gain,0, reverbMixer,0},
      {reverb,0,          reverbMixer,1},   
      {reverbMixer,       output}          //only using left channel(!)
    };
    
    int16_t grains[GRANULARLENGTH];
};

#endif
