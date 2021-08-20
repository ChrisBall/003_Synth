File myFile;

const int chipSelect = 10;

void setupSD() {
  SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
  SPI.setSCK(14);  // Audio shield has SCK on pin 14
}

void loadPreset(byte presetNumber) {
  if(presetNumber>127){
    return;
  }
    //init SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD init failed!");
    return;
  }
  Serial.println("SD init ok");
  
  //format the filename
  char filename[15] = "";
  sprintf(filename, "PRESETS/%03d.TXT", presetNumber);
  
  if (SD.exists(filename)) {
    byte lineCount = 0;
    myFile = SD.open(filename, FILE_READ);          
    while (myFile.available() && lineCount < 48) { 
      byte line[4];
      myFile.readBytesUntil('\r',line,4);
      byte controlValue;
      sscanf(line, "%d", &controlValue);
      if(lineCount!=44){  //ignore the master volume control
        onControlChange(3,lineCount,controlValue);
      }
      myFile.readBytesUntil('\n',line,4);
      lineCount++;
    }
    Serial.println("Preset loaded.");
  } else {
    Serial.println("No file!");
    return;
  }
}

void savePreset(byte presetNumber) {
  if(presetNumber>127){
    return;
  }
    //init SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD init failed!");
    return;
  }
  Serial.println("SD init ok");

  //check if the directory "presets" exists, create it if not
  myFile = SD.open("PRESETS");
  if (!myFile || !myFile.isDirectory()) {
    SD.mkdir("PRESETS");
    Serial.println("Presets directory created.");
  }
  myFile.close();

  //format the filename
  char filename[15] = "";
  sprintf(filename, "PRESETS/%03d.TXT", presetNumber);

  // delete any existing file with the same name
  if (SD.exists(filename)) {
    SD.remove(filename);
    Serial.println("Existing file deleted.");
  }

  //create the new preset
  Serial.print("Opening ");
  Serial.println(filename);
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    for (int i = 0; i < NUM_CONTROLS; i++) {
      myFile.println(controlValues[i]);
    }
    myFile.close();
  } else {
    Serial.print("File open failed.");
    return;
  }
  Serial.println("preset saved!");
}
