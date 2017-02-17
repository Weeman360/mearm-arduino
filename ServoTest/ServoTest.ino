#include "meArm.h"
#include <Servo.h>

#define byte_x 88
#define byte_y 89
#define byte_z 90
#define byte_click 36
#define byte_plus 43
#define byte_minus 45

meArm arm;
const int button = 2;
const int phots[] = { A0, A1, A2 };
const int limits[3][2] = {{ -110, 55}, {10, 110}, {0, 200}};
const int calaCount = 30;

int photosAvg[3] = {};
bool gripperOpen = false;
int incomingByte = 0;
int nextAxis = 0;

int arms[] = {30, 110, 150};

void setup() {
  Serial.begin(115200);

  pinMode(phots[0], INPUT);
  pinMode(phots[1], INPUT);
  pinMode(phots[2], INPUT);
  pinMode(button, INPUT);

  calibratePhotoResistors();
  //arm = new meArm();
  arm.begin(11, 10, 9, 6);
}

void loop() {
  readPhotos();
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    //    Serial.println(incomingByte);
    if (nextAxis != 0) {
      useNextAxis();
    } else {
      checkCurrentAxis();
    }
  }
}

void useNextAxis() {
  int moveBy = 5;
  if (incomingByte == byte_plus) {
    moveBy *= -1;
  }
  switch (nextAxis) {
    case byte_click: {
        toggleGripper();
        break;
      }
    case byte_x: {
        arms[0] = arms[0] + moveBy;
        arms[0] = constrain(arms[0], limits[0][0], limits[0][1]);
        break;
      }
    case byte_y: {
        arms[1] = arms[1] + moveBy;
        arms[1] =  constrain(arms[1], limits[1][0], limits[1][1]);
        break;
      }
    case byte_z: {
        arms[2] = arms[2] + moveBy;
        arms[2] = constrain(arms[2], limits[2][0], limits[2][1]);
        break;
      }
    default: {
        break;
      }
  }
  Serial.println();
  Serial.print(arms[0]);
  Serial.print("\t");
  Serial.print(arms[1]);
  Serial.print("\t");
  Serial.print(arms[2]);
  if (arm.isReachable(arms[0], arms[1], arms[2])) {
    arm.gotoPoint(arms[0], arms[1], arms[2]);
  }

  nextAxis = 0;
}

void checkCurrentAxis() {
  nextAxis = incomingByte;
}
void toggleGripper() {
  if (!gripperOpen) {
    arm.openGripper();
  } else {
    arm.closeGripper();
  }
  gripperOpen = !gripperOpen;
  delay(200);
}

void readPhotos() {
  int lr0 = analogRead(phots[0]);
  int lr1 = analogRead(phots[1]);
  int lr2 = analogRead(phots[2]);
  int x = map(photosAvg[0] - lr0, -100, 100, limits[0][0], limits[0][1]);
  int y = map(photosAvg[1] - lr1, -100, 100, limits[1][0], limits[1][1]);
  int z = map(photosAvg[2] - lr2, -100, 100, limits[2][0], limits[2][1]);

  if (arm.isReachable(x, y, z)) {
    arm.gotoPoint(x, y, z);
  }
  checkButton();
  delay(50);
}

void checkButton() {
  int pushed = digitalRead(button);
  Serial.println(pushed);
  if (pushed == HIGH) {
    toggleGripper();
  }
}

void calibratePhotoResistors() {
  for (int i = 0; i < calaCount; i++) {
    for (int j = 0; j < 3; j++) {
      int lightReading = analogRead(phots[j]);
      photosAvg[j] += lightReading;
      Serial.print(photosAvg[j]);
      Serial.print("\t");
    }
    Serial.println("\n============================");
    delay(100);
  }
  for (int i = 0; i < 3; i++) {
    photosAvg[i] /= calaCount;
    Serial.print(photosAvg[i]);
    Serial.print("\t");

  }
  Serial.println("\n============================");
}
