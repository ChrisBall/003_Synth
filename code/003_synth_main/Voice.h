#ifndef Voice_h
#define Voice_h
#include "Arduino.h"
#include <Audio.h>

#define REF_NOTE 69
#define REF_FREQ 440.0

class Voice {
  public:
    Voice();
    boolean                   state = false;
    byte                      note = 255;   //255 is "null" state - values 0-127 are MIDI note values
    byte                      velocity = 0;
    uint32_t                  releaseAge = 0;  //system time when voice was released

    AudioSynthWaveformDc      dc1;   //note frequency control
    AudioSynthWaveformSine    LFO;     //vibrato frequency
    AudioSynthWaveformDc      detuneWaveform1;    
    AudioSynthWaveformDc      detuneWaveform2; 
    AudioMixer4               modulationInput1;
    AudioMixer4               modulationInput2;
    AudioSynthWaveformModulated  waveform1; 
    AudioSynthWaveformModulated  waveform2; 
    AudioSynthNoiseWhite      noise1;
    AudioMixer4               voiceMixer;
    AudioSynthWaveformDc      dc2;  //used for a constant input to the filter envelope.
    AudioEffectEnvelope       envelope1;  //amplitude envelope
    AudioEffectEnvelope       envelope2;  //filter envelope
    AudioFilterStateVariable  filter1;
    AudioMixer4               filterMixer;
    AudioAmplifier            output;
    AudioConnection           patchCord[18] = {
      {dc1,0, modulationInput1,0},
      {dc1,0, modulationInput2,0},
      {LFO,0, modulationInput1,1},
      {LFO,0, modulationInput2,1},
      {detuneWaveform1,0, modulationInput1,2},
      {detuneWaveform2,0, modulationInput2,2},
      {modulationInput1,0, waveform1,0},
      {modulationInput2,0, waveform2,0},
      {waveform1,0,voiceMixer,0},
      {waveform2,0,voiceMixer,1},
      {dc2, envelope2},
      {voiceMixer, envelope1},
      {envelope1, 0, filter1, 0},  //signal input to filter
      {envelope2, 0, filter1, 1},  //control input to filter
      {filter1,0,filterMixer,0},
      {filter1,1,filterMixer,1},
      {filter1,2,filterMixer,2},
      {filterMixer,0, output,0}
    };
    
    void                      setADSRAttack(float MILLIS){envelope1.attack(MILLIS);envelope2.attack(MILLIS);}
    void                      setADSRDecay(float MILLIS){envelope1.decay(MILLIS);envelope2.decay(MILLIS);}
    void                      setADSRSustain(float LEVEL){envelope1.sustain(LEVEL);envelope2.sustain(LEVEL);}
    void                      setADSRRelease(float MILLIS){envelope1.release(MILLIS);envelope2.release(MILLIS);}
    void                      setFilterCutoff(float FREQ){filter1.frequency(FREQ);}
    void                      setFilterQ(float Q){filter1.resonance(Q);}
    void                      setFilterOctaves(float OCTAVES){filter1.octaveControl(OCTAVES);}    
    void                      setFilterMix(float MIX){if(MIX<=1){filterMixer.gain(0,1-MIX);filterMixer.gain(1,MIX);filterMixer.gain(2,0);}else if(MIX<=2){filterMixer.gain(0,0);filterMixer.gain(1,2-MIX);filterMixer.gain(2,MIX-1);};}    
    void                      setWaveform1Type(uint8_t TYPE){waveform1.begin(TYPE);}
    void                      setWaveform2Type(uint8_t TYPE){waveform2.begin(TYPE);}
    void                      setWaveform1Detune(float DETUNE){detuneWaveform1.amplitude(DETUNE,20);}
    void                      setWaveform2Detune(float DETUNE){detuneWaveform2.amplitude(DETUNE,20);}
    void                      setWaveform1Gain(float GAIN){voiceMixer.gain(0,GAIN);}
    void                      setWaveform2Gain(float GAIN){voiceMixer.gain(1,GAIN);}
    void                      setLFOAmplitude(float AMP){LFO.amplitude(AMP);}
    void                      setLFOFrequency(float FREQ){LFO.frequency(FREQ);}
    void                      setOutputGain(float GAIN){output.gain(GAIN);}

    void noteOn(byte MIDINOTE, byte MIDIVELOCITY, float GLIDETIME, boolean CONT);
    void noteOff(byte MIDINOTE, byte MIDIVELOCITY);


    boolean isActive(){  //returns whether there is any activity on the voice at all
      return state || envelope1.isActive() || envelope2.isActive();
    }
    void setState(boolean STATE){ 
      state=STATE;
    }
    boolean getState(){ //returns whether the note is on or off, but the other aspects of the voice (release phase) could still be active
      return state;
    }
};

#endif
