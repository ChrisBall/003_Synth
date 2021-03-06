import themidibus.*; //Import the midibus library

Pot[][] pots;
int potArrayX=12, potArrayY=4;

Pot preset;

MidiBus myBus; // The MidiBus

void setup() {
  size(1000, 400);

  //init midi bus
  MidiBus.list();
  myBus = new MidiBus(this, 0, "loopMIDI Port");

  //init pots
  pots=new Pot[potArrayX][potArrayY];
  int CCCount=0;
  for (int i=0; i<pots.length; i++) {
    for (int j=0; j<pots[0].length; j++) {
      pots[i][j]=new Pot(33.33+66.66*i, 50+100*j, 50, CCCount);
      CCCount++;
    }
  }
  
  pots[0][0].name="GLIDE";
  pots[0][1].name="POLY/\nMONO/\nMONO CONT.";
  pots[0][2].name="OSC1 TYP";
  pots[0][3].name="OSC2 TYP";

  pots[1][0].name="ARP MODE";
  pots[1][1].name="ARP RATE";
  pots[1][2].name="OSC1 DET";
  pots[1][2].value=0.5;
  pots[1][3].name="OSC2 DET";
  pots[1][3].value=0.5;

  pots[2][0].name="VIB AMP";
  pots[2][1].name="VIB RATE";
  pots[2][2].name="OSC1 GAIN";
  pots[2][3].name="OSC2 GAIN";

  pots[3][0].name="VCF TYP";
  pots[3][1].name="VCF Q";
  pots[3][2].name="VCF MAX";
  pots[3][3].name="VCF AMNT";

  pots[4][0].name="A";
  pots[4][1].name="D";
  pots[4][2].name="S";
  pots[4][3].name="R";

  pots[5][0].name="EXT IN";
  pots[5][1].name="SYN IN";
  pots[5][2].name="TREM FRQ";
  pots[5][3].name="TREM AMP";

  pots[6][0].name="DX SHAPE";
  pots[6][1].name="DX GAIN";
  pots[6][2].name="BTCRSH";
  pots[6][3].name="BTCRSH FS";

  pots[7][0].name="GRN SPD";
  pots[7][1].name="GRN MIX";
  pots[7][2].name="RING FRQ";
  pots[7][3].name="RING MIX";

  pots[8][0].name="TAPE IN";
  pots[8][1].name="TAPE LP";
  pots[8][2].name="TAPE SPD";
  pots[8][3].name="TAPE FDBCK";

  pots[9][0].name="FILT TYPE";
  pots[9][1].name="Q";
  pots[9][2].name="CUTOFF";
  pots[9][3].name="MIX";

  pots[10][0].name="DX2 SHAPE";
  pots[10][1].name="DX2 GAIN";
  pots[10][2].name="RVB AMNT";
  pots[10][3].name="RVB MIX";

  pots[11][0].name="MASTER";
  pots[11][1].name="CLEAN EXT.";
  pots[11][2].name="CLEAN SYNTH";
  pots[11][3].name="FX SYNTH/EXT";
  
  //preset=new Pot();
}

void draw() {
  background(220);

  for (int i=0; i<pots.length; i++) {
    for (int j=0; j<pots[0].length; j++) {
      pots[i][j].draw();
    }
  }
}

void mousePressed() {
  for (int i=0; i<pots.length; i++) {
    for (int j=0; j<pots[0].length; j++) {
      pots[i][j].pressed();
    }
  }
}

void mouseDragged() {
  for (int i=0; i<pots.length; i++) {
    for (int j=0; j<pots[0].length; j++) {
      pots[i][j].dragged();
    }
  }
}

void mouseReleased() {
  for (int i=0; i<pots.length; i++) {
    for (int j=0; j<pots[0].length; j++) {
      pots[i][j].released();
    }
  }
}

void keyPressed() {
  if (key=='0') {
    println("sending all");
    for (int i=0; i<pots.length; i++) {
      for (int j=0; j<pots[0].length; j++) {
        pots[i][j].sendValue();
      }
    }
  }else if(key=='l'){
    myBus.sendMessage(0xC1,14);
  }
}

void noteOn(int channel, int pitch, int velocity) {
  myBus.sendNoteOn(channel, pitch, velocity);
  println();
  println("Note On:");
  println("--------");
  println("Channel:"+channel);
  println("Pitch:"+pitch);
  println("Velocity:"+velocity);
}

void noteOff(int channel, int pitch, int velocity) {
  myBus.sendNoteOff(channel-1, pitch, velocity);
  // Receive a noteOff
  println();
  println("Note Off:");
  println("--------");
  println("Channel:"+channel);
  println("Pitch:"+pitch);
  println("Velocity:"+velocity);
}
