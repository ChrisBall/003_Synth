#include "Arduino.h"
#include "VoiceManager.h"


VoiceManager::VoiceManager() {
  //setup voice  mixer structure
  for (int i = 0; i < NumberOfVoices; i++) {
    patchCablesStage1[i] = new AudioConnection(voices[i].output, 0, mixerStage1[i / 4], i % 4);
    mixerStage1[i / 4].gain(i % 4, 0.0625); //1/16th voice level
  }

  for (int i = 0; i < NumberOfVoices / 4; i++) {
    patchCablesStage2[i] = new AudioConnection(mixerStage1[i], 0, mixerStage2, i % 4);
  }

  //initialise arrays
  for (int i = 0; i < NUM_KEYS; i++) {
    keyStatus[i] = 255;
    arpKeyOrder[i] = 255;
    sustainedNotes[i] = false;
  }
}


int VoiceManager::findFreeVoice(byte noteNumber) {
  //will return the index of the oldest "released note" or -1 for no available voices
  int oldestIndex = -1;
  uint32_t releaseAge = -1;                             //initialising this to a high value. May go wrong if the device is left on for 50 days (!?)
  for (int i = 0; i < NumberOfVoices; i++) {   //usually NumberOfVoices
    if (!voices[i].getState()) { //for poly
      if (voices[i].releaseAge < releaseAge) {
        releaseAge = voices[i].releaseAge;
        oldestIndex = i;
      }
    }
  }
  return oldestIndex;
}


void VoiceManager::update() {
  if (arpPattern > 0 && numKeys > 1) {
    if (millis() >= arpNextNoteTime) {
      arpNextNoteTime += int(1000.0 / arpRate);
      arpPatternUpdate();
    }
  }
}


void VoiceManager::arpPatternUpdate(){
  switch (arpPattern) {
        case 1: //UP
          for (byte i = lastNoteNumber + 1; i != lastNoteNumber; i = (i + 1) % 128) {  //find the next active note in an upward direction, wrapping around when necessary
            if (keyStatus[i] < 128) {
              arpNextNote(i);
              return;
            }
          }
          break;
          
        case 2:  //DOWN
          for (byte i = lastNoteNumber - 1; i != lastNoteNumber; i = (i - 1) % 128) {  //find the next active note in an downward direction, wrapping around when necessary
            if (keyStatus[i] < 128) {
              arpNextNote(i);
              return;
            }
          }
          break;
          
        case 3:  //UPDOWN
          if (arpUp) {
            for (byte i = lastNoteNumber + 1; i != 128; i++) {  //find the next active note in an upward direction
              if (keyStatus[i] < 128) {
                arpNextNote(i);
                return;
              }
            }
            arpUp = false;
            arpPatternUpdate();
          } else {
            for (byte i = lastNoteNumber - 1; i != 255; i--) {  //find the next active note in an downward direction
              if (keyStatus[i] < 128) {
                arpNextNote(i);
                return;
              }
            }
            arpUp = true;  //if not found, switch direction and try again
            arpPatternUpdate();
          }
          break;
          
        case 4:  //RANDOM
          arpKeyOrderIndex = random(0, numKeys);
          arpNextNote(arpKeyOrder[arpKeyOrderIndex]);
          break;
          
        case 5:  //KEYORDER
          arpKeyOrderIndex++;
          arpKeyOrderIndex %= numKeys;
          arpNextNote(arpKeyOrder[arpKeyOrderIndex]);
          break;
          
      }
}


void VoiceManager::arpNextNote(byte noteNumber) {  //this function simply stops the current note playing and plays the next
  noteOff(lastNoteNumber, 0);
  lastNoteNumber = noteNumber;
  noteOn(lastNoteNumber, lastNoteVelocity);
}

void VoiceManager::onNoteOn(byte noteNumber, byte velocity) {  //note activity/arpeggiator is handled here
  if (sustainOn && keyStatus[noteNumber] != 255) {
    keyStatus[noteNumber] = 255;
    numKeys--;
    sustainedNotes[noteNumber] = false;
    noteOff(noteNumber, 0);
  }

  if (keyStatus[noteNumber] == 255) {
    keyStatus[noteNumber] = velocity;
    arpKeyOrder[numKeys] = noteNumber;
    numKeys++;

    if (numKeys == 1 && arpPattern > 0) {
      arpStartTime = millis();
      arpNextNoteTime = arpStartTime + int(1000.0 / arpRate);
      noteOn(noteNumber, velocity);
      lastNoteNumber = noteNumber;
      lastNoteVelocity = velocity;
    } else if (!arpPattern) {
      noteOn(noteNumber, velocity);
      lastNoteNumber = noteNumber;
      lastNoteVelocity = velocity;
    }
  }
}

void VoiceManager::onNoteOff(byte noteNumber, byte velocity) { //note activity/arpeggiator is handled here
  if (sustainOn) {
    //if a note off is received, but sustain is active, it is held in a queue to be note-offed later.
    sustainedNotes[noteNumber] = true;
  } else {
    keyStatus[noteNumber] = 255;
    arpKeyOrder[numKeys] = 255;   //doesn't work except in simplest cases - should search array for noteNumber, and remove while rearranging array.
    if (numKeys > 0) {
      numKeys--;
    }

    if (numKeys == 0 && arpPattern > 0) {
      noteOff(lastNoteNumber, 0);
      lastNoteNumber = 255;
      lastNoteVelocity = 255;
    } else if (!arpPattern) {
      noteOff(noteNumber, velocity);
      lastNoteNumber = 255;
      lastNoteVelocity = 255;
    }
  }
}

void VoiceManager::noteOn(byte noteNumber, byte velocity) {  //manages voice stealing and mono/poly
  if (monoMode > 0) {
    voices[0].noteOn(noteNumber, velocity, glideTime, monoMode == 2);

  } else {
    int voiceIndex = findFreeVoice(noteNumber);
    if (voiceIndex == -1) {      //if we didn't find a free voice
      return;                   //take no action
    } else {
      voices[voiceIndex].noteOn(noteNumber, velocity, 0, monoMode == 2);
    }

  }
}

void VoiceManager::noteOff(byte noteNumber, byte velocity) { //manages voice stealing and mono/poly
  if (monoMode > 0) {
    if (numKeys > 0) {
      for (int i = NUM_KEYS - 1; i >= 0; i--) { //default to highest active note
        if (keyStatus[i] < 255) {
          voices[0].noteOn(i, keyStatus[i], glideTime, monoMode == 2);
          return;
        }
      }
    } else {
      voices[0].noteOff(noteNumber, velocity);
    }

    //also turn off any voices (above 0)
    for (int i = 1; i < NumberOfVoices; i++) {
      voices[i].noteOff(noteNumber, velocity);
    }

  } else {
    for (int i = 0; i < NumberOfVoices; i++) {
      if (voices[i].getState() && voices[i].note == noteNumber) {
        voices[i].noteOff(noteNumber, velocity);
      }
    }
  }
}
