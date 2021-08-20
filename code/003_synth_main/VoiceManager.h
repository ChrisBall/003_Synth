#ifndef VoiceManager_h
#define VoiceManager_h
#include "Arduino.h"
#include <Audio.h>
#include "Voice.h"

#define NUM_KEYS 128

class VoiceManager {

  public:
    VoiceManager();

    static const uint8_t MaxVoices = 16; //max 16
    uint8_t NumberOfVoices = MaxVoices;
    Voice voices[MaxVoices];
    AudioMixer4 mixerStage1[MaxVoices / 4];
    AudioMixer4 mixerStage2;
    AudioAmplifier output;
    AudioConnection *patchCablesStage1[MaxVoices];
    AudioConnection *patchCablesStage2[MaxVoices / 4];
    AudioConnection patchCables[1] = {{mixerStage2, output}};
    int findFreeVoice(byte noteNumber);
    byte monoMode = 0;
    int numKeys = 0;
    byte keyStatus[NUM_KEYS];
    byte lastNoteNumber = 255;
    byte lastNoteVelocity = 255;
    float glideTime = 50;

    float filterMax = 14500;
    float filterAmount = 0.5;

    byte arpPattern = 0; //0=off, 1=up, 2=down, 3=up/down, 4=random, 5=as played
    float arpRate = 8; //in hz
    byte arpKeyOrder[NUM_KEYS];
    byte arpKeyOrderIndex = 0;
    uint32_t arpStartTime = 0;
    uint32_t arpNextNoteTime = 0;
    boolean arpUp = true;

    boolean sustainOn=false;
    boolean sustainedNotes[NUM_KEYS];

    void update();   //must be called regularly to update the arpeggiator
    void arpPatternUpdate();  //arpeggiator modes
    void arpNextNote(byte noteNumber);
    void onNoteOn(byte noteNumber, byte velocity);
    void onNoteOff(byte noteNumber, byte velocity);
    void noteOn(byte noteNumber, byte velocity);
    void noteOff(byte noteNumber, byte velocity);

    void setSustain(byte VALUE){
      if(VALUE>0){
        sustainOn=true;
      }else{
        sustainOn=false;
        for(int i=0;i<NUM_KEYS;i++){
          if(sustainedNotes[i]){
            sustainedNotes[i]=false;
            onNoteOff(i,0);
          }
        }
      }
    }

    void setADSRAttack(float MILLIS) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setADSRAttack(MILLIS);
      }
    }
    void setADSRDecay(float MILLIS) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setADSRDecay(MILLIS);
      }
    }
    void setADSRSustain(float LEVEL) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setADSRSustain(LEVEL);
      }
    }
    void setADSRRelease(float MILLIS) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setADSRRelease(MILLIS);
      }
    }
    void setFilterQ(float Q) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setFilterQ(Q); //Q also causes clipping! would need to attenuate voice source before entering voice filter
      }
    }
    void setFilterMax(float AMOUNT) {
      filterMax = 100 + 14400 * AMOUNT;
      setFilterAmount(filterAmount);
    }
    void setFilterAmount(float AMOUNT) {
      filterAmount = AMOUNT;
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setFilterOctaves(AMOUNT * 7);
        voices[i].setFilterCutoff(filterMax / pow(2, AMOUNT * 7));
      }
    }
    void setFilterMix(float MIX) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setFilterMix(MIX);
      }
    }
    void setWaveform1Type(uint8_t TYPE) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setWaveform1Type(TYPE);
      }
    }
    void setWaveform2Type(uint8_t TYPE) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setWaveform2Type(TYPE);
      }
    }
    void setWaveform1Detune(float DETUNE) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setWaveform1Detune(DETUNE);
      }
    }
    void setWaveform2Detune(float DETUNE) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setWaveform2Detune(DETUNE);
      }
    }
    void setWaveform1Gain(float GAIN) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setWaveform1Gain(GAIN);
      }
    }
    void setWaveform2Gain(float GAIN) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setWaveform2Gain(GAIN);
      }
    }
    void setLFOAmplitude(float AMP) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setLFOAmplitude(AMP);
      }
    }
    void setLFOFrequency(float FREQ) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setLFOFrequency(FREQ);
      }
    }
    void setVoiceOutputGain(float GAIN) {
      for (int i = 0; i < NumberOfVoices; i++) {
        voices[i].setOutputGain(GAIN);
      }
    }
    void setGlideTime(float GLIDETIME) {
      glideTime = GLIDETIME;
    }
    void setMonoMode(byte STATE) {
      monoMode = STATE;
    }
    void setArpRate(float HZ) {
      arpRate = HZ;
    }
    void setArpMode(byte MODE) {
      arpPattern = MODE;
    }
};

#endif
