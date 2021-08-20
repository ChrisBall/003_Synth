/*
   new structure plan
   nano every manages:
   -reading pots
   -display (probably)
   -a set number of presets (255?) but these are actually stored on the teensy 4 SD card as txt files. This opens up the possibility of storing the tape buffer too.

   a "store preset" command simply asks the teensy to store it's pot state (latched or otherwise)
   maybe update pots more when moved??
*/

#include <Encoder.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#define MUX_ENABLE 6
#define MUX_S0 7
#define MUX_S1 8
#define MUX_S2 5
#define MUX_S3 4

#define ENCODER_P0 3
#define ENCODER_P1 2
#define ENCODER_BUTTON 9

#define SAVE_BUTTON A3

#define TOTAL_POTS 48
#define MUX_POTS 16
#define MUX_PINS 3

const char potLabel[][11] = {
  "GLIDE     ", "MODE      ", "OSC1 TYPE ", "OSC2 TYPE ",
  "ARP MODE  ", "ARP RATE  ", "OSC1 DTN  ", "OSC2 DTN  ",
  "VIB AMP   ", "VIB RATE  ", "OSC1 LVL  ", "OSC2 LVL  ",
  "VCF TYPE  ", "VCF Q     ", "VCF FREQ  ", "VCF AMNT  ",

  "ATTACK    ", "DECAY     ", "SUSTAIN   ", "RELEASE   ",
  "EXT FX IN ", "SYN FX IN ", "TREM FREQ ", "TREM AMP  ",
  "DX SHAPE  ", "DX GAIN   ", "BTCRSH BIT", "BTCRSH FS ",
  "GRN SPD   ", "GRN MIX   ", "RING FREQ ", "RING MIX  ",

  "TAPE IN   ", "TAPE LPF  ", "TAPE SPD  ", "TAPE FDBCK",
  "FILT TYPE ", "FILT Q    ", "FILT FREQ ", "FILT MIX  ",
  "DX2 SHAPE ", "DX2 GAIN  ", "RVB AMNT  ", "RVB MIX   ",
  "MASTER    ", "EXT MIX   ", "SYN MIX   ", "FX MIX    "
};

const byte startupLabelLength=22;
const char startupLabel[22][11] {
  "EAT VEG   ",
  "CALL MUM  ",
  "LOVE YOU  ",
  "GOOD FACE ",
  "GOOD EGG  ",
  "YOU WON   ",
  "FUN TIMES ",
  "SYN CITY  ",
  "SIT UP    ",
  "WASH HANDS",
  "TIDY UP   ",
  "STOP IT   ",
  "START IT  ",
  "TEA-TIME  ",
  "EAT WELL  ",
  "DO YOGA   ",
  "AL DENTE  ",
  "ITS ALL OK",
  "CALM DOWN ",
  "CRONKS    ",
  "NICE TIME ",
  "CUDDLE ME "
};

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //2 page buffers (faster than 1, slower than F)
Encoder myEnc(ENCODER_P0, ENCODER_P1);

float reactivity = 0.0025;
float EMA = 1;
float floatValues[TOTAL_POTS];
byte potValues[TOTAL_POTS];

byte muxPins[MUX_PINS] = {A2, A1, A0};
byte potOrder[MUX_POTS] = {9, 8, 7, 6, 11, 10, 5, 4, 12, 13, 2, 3, 14, 15, 0, 1};

byte encPosition = 0;

byte displayFlag = 255;

#define NUM_BUTTONS 2
byte buttonPins[NUM_BUTTONS]  ={9,A3};
byte buttonStates[NUM_BUTTONS]={0,0};

boolean MIDIEnabled = false;
boolean loadEnabled = false;
boolean saveEnabled = false;

void setup() {
  Serial1.begin(31250);
  u8g2.begin();
  displayStartupMessage();
  setupPins();
  initPots();
  MIDIEnabled = true;
}

void loop() {
  displayFlag = 255;

  updateEncoder();
  buttonUpdate();
  updatePots();

  if (displayFlag < 255) {
    displayData(potValues[displayFlag], potLabel[displayFlag]);
  }
}

void setupPins() {
  pinMode(9, INPUT_PULLUP); //encoder switch (load)
  pinMode(A3, INPUT_PULLUP);//tactile switch (save)

  pinMode(MUX_ENABLE, OUTPUT);
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);

  digitalWrite(MUX_ENABLE, 0);
  digitalWrite(MUX_S0, 0);
  digitalWrite(MUX_S1, 0);
  digitalWrite(MUX_S2, 0);
  digitalWrite(MUX_S3, 0);
}
