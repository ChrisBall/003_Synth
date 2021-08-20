class Pot {
  PVector pos;
  String name="";
  float value=0;
  float size;
  boolean selected=false;
  int midiValue=1;
  int statusByte=0xB1;
  int CCValue=0;

  Pot(float X, float Y, float SIZE, int CC) {
    pos=new PVector(X, Y);
    size=SIZE;
    CCValue=CC;
  }
  void draw() {
    stroke(0);
    fill(255);
    strokeWeight(size*.05);
    ellipse(pos.x, pos.y, size, size);
    line(pos.x, pos.y, pos.x+size*.5*cos(2*PI/3+value*PI*10/6), pos.y+size*.5*sin(2*PI/3+value*PI*10/6));
    fill(0);
    noStroke();
    textAlign(CENTER, CENTER);
    text(name, pos.x, pos.y+35);
  }

  void pressed() {
    if (dist(pos.x, pos.y, mouseX, mouseY)<size*.5) {
      selected=true;
    }
  }

  void released() {
    selected=false;
  }

  void dragged() {
    if (selected) {
      value+=float((mouseX-pmouseX)+(pmouseY-mouseY))*0.005;
      value=constrain(value, 0, 1);

      if (int(value*127)!=midiValue) {
        midiValue=int(value*128);
        sendValue();
      }
    }
  }

  void sendValue() {

    println();
    println("Controller Change:");
    println("--------");
    println("Channel:"+0);
    println("Number:"+CCValue);
    println("Value:"+midiValue);
    myBus.sendControllerChange(1, CCValue, midiValue);
  }
}
