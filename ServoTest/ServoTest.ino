#include "meArm.h"
#include <Servo.h>

#define byte_x 88
#define byte_y 89
#define byte_z 90
#define byte_claw 36
#define byte_plus 43
#define byte_minus 45
#define byte_power 115
#define byte_photo 108

meArm arm;
const int button = 2;
const int servos[] = {11, 10, 9, 6};
const int phots[] = { A0, A1, A2 };
const int limits[3][2] = {{ 10, 180}, {10, 180}, {10, 180}};
const int calaCount = 30;

int photosAvg[3] = {};
bool photosIsActive = false;
bool gripperOpen = false;
int incomingByte = 0;
int nextAxis = 0;

int arms[] = {110, 110, 150};

void setup() {
  Serial.begin(115200);

  pinMode(phots[0], INPUT);
  pinMode(phots[1], INPUT);
  pinMode(phots[2], INPUT);
  pinMode(button, INPUT);

  arm.begin(servos[0], servos[1], servos[2], servos[3]);
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    performInputChecks(incomingByte);
  }
  readPhotos();
}

void performInputChecks(int incoming) {
  checkTogglePower(incoming);
  checkTogglePhotos(incoming);
  checkToggleClaw(incoming);
  if (nextAxis != 0) {
    useNextAxis();
  } else {
    checkCurrentAxis();
  }
}

void checkTogglePower(int incoming) {
  if (incoming == byte_power) {
    if (arm.isPoweredOn()) {
      arm.stopArm();
    } else {
      arm.begin(servos[0], servos[1], servos[2], servos[3]);
    }
  }
}

void checkTogglePhotos(int incoming) {
  if (incoming == byte_photo) {
    if (!photosIsActive) {
      startPhotoResistors();
    } else {
      stopPhotoResistors();
    }
  }
}

void checkToggleClaw(int incoming){
  if (incoming == byte_claw) {
    toggleGripper();
  }
}

void useNextAxis() {
  int moveBy = 5;
  if (incomingByte == byte_plus) {
    moveBy *= -1;
  }
  switch (nextAxis) {
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
  Serial.print(arms[0]);
  Serial.print("\t");
  Serial.print(arms[1]);
  Serial.print("\t");
  Serial.println(arms[2]);

  /*if (arm.isReachable(arms[0], arms[1], arms[2])) {
    arm.gotoPoint(arms[0], arms[1], arms[2]);
    }*/

  arm.unsafeGoToPoint(arms[0], arms[1], arms[2]);
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
  if (photosIsActive) {
    const int range = 100;
    int lr0 = analogRead(phots[0]);
    int lr1 = analogRead(phots[1]);
    int lr2 = analogRead(phots[2]);
    int x = map(photosAvg[0] - lr0, -range, range, limits[0][0], limits[0][1]);
    int y = map(photosAvg[1] - lr1, -range, range, limits[1][0], limits[1][1]);
    int z = map(photosAvg[2] - lr2, -range, range, limits[2][0], limits[2][1]);

    //  if (arm.isReachable(x, y, z)) {
    //    arm.gotoPoint(x, y, z);
    //  }
    Serial.println("R PR");
    delay(50);
  }
}

void checkButton() {
  int pushed = digitalRead(button);
  //  Serial.println(pushed);
  if (pushed == HIGH) {
    toggleGripper();
  }
}

void startPhotoResistors() {
  for (int i = 0; i < calaCount; i++) {
    for (int j = 0; j < 3; j++) {
      int lightReading = analogRead(phots[j]);
      photosAvg[j] += lightReading;
//      Serial.print(photosAvg[j], DEC);
//      Serial.print("\t");
    }
//    Serial.println("\n============================");
    delay(100);
  }
  for (int i = 0; i < 3; i++) {
    photosAvg[i] /= calaCount;
    Serial.print(photosAvg[i], DEC);
    Serial.print("\t");

  }
  Serial.println("\n============================");
  photosIsActive = true;
}

void stopPhotoResistors() {
  photosIsActive = false;
}

