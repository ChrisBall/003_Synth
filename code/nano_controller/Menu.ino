void displayData(byte value, char* text) {
  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_fur20_tr);  //"fur" is best match for font
  u8g2.setCursor(0, 31);
  u8g2.print(u8x8_u8toa(value, 3));
  u8g2.setCursor(0, 63);
  u8g2.setFont(u8g2_font_fur14_tr);  //"fur" is best match for font
  u8g2.print(text);
  u8g2.sendBuffer();          // transfer internal memory to the display

  loadEnabled = false;
  saveEnabled = false;
}

void displayStartupMessage(){
  byte rng=analogRead(A7)*analogRead(A6) + analogRead(A7) + analogRead(A6);
  rng%=startupLabelLength;
  displayData(3, startupLabel[rng]);
}

void updateEncoder() {
  byte newPosition = byte(myEnc.read() / 4) % 128;
  if (newPosition != encPosition) {
    encPosition = newPosition;
    displayData(encPosition, "LOAD PRST");
    loadEnabled = true;
  }
}

void buttonUpdate() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    boolean buttonState = digitalRead(buttonPins[i]);
    if ((buttonState == LOW) && (buttonStates[i] == 0)) {
      buttonStates[i] = 3;
    }else if (buttonStates[i] == 3) {
      buttonStates[i] = 2;
    }else if ((buttonState == HIGH) && (buttonStates[i] == 2)) {
      buttonStates[i] = 1;
    }else if (buttonStates[i] == 1) {
      buttonStates[i] = 0;
    }
  }
  buttonActions();
}

void buttonActions(){
  if(buttonStates[0]==1 && loadEnabled){
    sendMIDImessage(0xC2,encPosition);
    displayData(encPosition,"LOADED");
  }else if(buttonStates[0]==1){
    sendMIDImessage(0xC2,encPosition);
    displayData(encPosition,"LOAD PRST");
    loadEnabled = true;
  }
  
  if(buttonStates[1]==1 && saveEnabled){
    sendMIDImessage(0xCF,encPosition);
    displayData(encPosition,"DID IT");
  }else if(buttonStates[1]==1){
    displayData(encPosition,"SURE?");
    saveEnabled=true;
  }
}
