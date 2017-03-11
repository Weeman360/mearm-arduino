import net.java.games.input.*;
import org.gamecontrolplus.*;
import org.gamecontrolplus.gui.*;
import processing.serial.*;

Serial myPort;
int rectWidth;
// This one is essential
ControlIO control;
// You will need some of the following depending on the sketch.

// A ControlDevice might be a joystick, gamepad, mouse etc.
ControlDevice device;

// A device will have some combination of buttons, hats and sliders
ControlButton button;
ControlSlider slX;
ControlSlider slY;
ControlSlider slZ;

//byte references
static int claw = 36;
static int moveX = 88;
static int moveY = 89;
static int moveZ = 90;
static int power = 115;
static int photos = 108;

void setup() {
  size(640, 360);
  noStroke();
  background(0);
  rectWidth = width/4;

  control = ControlIO.getInstance(this);
  device = control.getMatchedDevice("mouseMearm");
  slX = device.getSlider("X");
  slY = device.getSlider("Y");
  slZ = device.getSlider("Z");
  button = device.getButton("BtnLeft");

  myPort = new Serial(this, "COM3", 9600);
  myPort.bufferUntil('\n');
}

void draw() { 
  readSerialPort();
}

void checkMouseMovement() {  
  int x = int(slX.getValue());
  if (x != 0) {    
    writeDirection(moveX, x > 0);
  }

  int y = int(slY.getValue());
  if (y != 0) {    
    writeDirection(moveY, y > 0);
  }

  int z = int(slZ.getValue());
  if (z != 0) {    
    writeDirection(moveZ, z > 0);
  }
}


void writeDirection(int dir, boolean positive) {
  myPort.write(dir);
  if (positive) {
    myPort.write('+');
  } else {
    myPort.write('-');
  }
}

void toggleClaw(){
    myPort.write(claw);
}

void togglePower() {
  myPort.write(power);
}

void togglePhotoResistors() {
  myPort.write(photos);
}

void keyPressed() {
  if (key == 'a') 
    writeDirection(moveX, false);  

  if (key == 'd') 
    writeDirection(moveX, true);  

  if (key == 'w') 
    writeDirection(moveY, true);

  if (key == 's')
    writeDirection(moveY, false);

  if (key == 'r') 
    writeDirection(moveZ, true);

  if (key == 'f') 
    writeDirection(moveZ, false);

  if (key == 'p')
    togglePower();

  if (key == 'l') {
    togglePhotoResistors();
  }
  
  if (key == 'c'){
    toggleClaw();
  }
}

void readSerialPort() {
  if (myPort.available() > 0) {
    print(myPort.readChar());
  }
}