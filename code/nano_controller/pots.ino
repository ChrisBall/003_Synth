// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void initPots() {  
  //reduce sampling capacitance
  //sbi(ADC0.CTRLC, 6);
  
  // set ADC clock prescale to 16
  sbi(ADC0.CTRLC, 2);
  cbi(ADC0.CTRLC, 1);
  cbi(ADC0.CTRLC, 0);

  // set ADC to accumulate 4 samples
  cbi(ADC0.CTRLB, 2);
  sbi(ADC0.CTRLB, 1);
  cbi(ADC0.CTRLB, 0);
  
  //this means we're accumulating 4 fast, 10 bit readings (1024*4=4096) for each analogRead()

  //4x 4 10 bit readings best so far
  for (int i = 0; i < 100; i++) {
    updatePots();
  }
}

void updatePots() {
  for (int i = 0; i < MUX_POTS; i++) {
    setMuxPin(potOrder[i]);
    for (int j = 0; j < MUX_PINS; j++) {
      byte potNum = MUX_POTS * j + i;

      //take a reading
      float reading = float(analogReadAverage(muxPins[j], 2));

      if (abs(reading - floatValues[potNum]) > 2.5) { //if it's changing (significantly)
        float reading = float(analogReadAverage(muxPins[j], 4)); //take & average multiple readings
      }
      floatValues[potNum] = EMA * floatValues[potNum] + (1 - EMA) * reading;

      //filter it
      EMA = constrain(1 - abs(reading - floatValues[potNum]) * reactivity, 0, 1);

      //convert to byte
      byte newMidiValue = int(floatValues[potNum]) >> 5;
      if (newMidiValue != potValues[potNum]) {
        potValues[potNum] = newMidiValue;

        if(MIDIEnabled){
          displayFlag = potNum;
          sendMIDImessage(0xB2,potNum,potValues[potNum]); //sending CC on MIDI channel 2
        }
      }
    }
  }
}

void setMuxPin(byte PIN) {
  //could be optimised for atmega4809 (a lot)
  digitalWrite(MUX_S0, bitRead(PIN, 0));
  digitalWrite(MUX_S1, bitRead(PIN, 1));
  digitalWrite(MUX_S2, bitRead(PIN, 2));
  digitalWrite(MUX_S3, bitRead(PIN, 3));
  //delayMicroseconds(50); //allow signal to settle(?)
}

int analogReadAverage(byte PORT, int NUM) {
  int analogSum = 0;
  for (int i = 0; i < NUM; i++) {
    analogSum += analogRead(PORT);
  }
  return analogSum / NUM;
}
