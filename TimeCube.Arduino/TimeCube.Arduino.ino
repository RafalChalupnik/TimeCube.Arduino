#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  bool calibrationMode = shouldCalibrate();

  if (calibrationMode) {
    calibrate();
  } else {
    track();
  }

  delay(1000);
}

bool shouldCalibrate() {
  if (Serial.available() <= 0) {
    return false;
  }

  String receivedString = Serial.readString();
  return receivedString.indexOf("calibrate") != -1;
}

void calibrate() {
  while (Serial.available() <= 0) {
    delay(100);
  }
  
  String receivedString = Serial.readString();

  if (receivedString.indexOf("calibrated") != -1) {
    Serial.println("Setting calibrated to true...");
    setCalibrated(true);
  } else if (receivedString.indexOf("notCalibrated") != 1) {
    Serial.println("Setting calibrated to false...");
    setCalibrated(false);
  } else {
    Serial.println("Command not recognized.");
  }
}

void track() {
  bool calibrated = getCalibrated();
  Serial.println("Reading Calibrated from EEPROM:");
  Serial.println(calibrated);
}

// Memory handling

const int CALIBRATED_ADDRESS = 0;
const int CALIBRATED_FLAG = 178;

bool getCalibrated() {
  int flag = EEPROM.read(CALIBRATED_ADDRESS);
  return flag == CALIBRATED_FLAG;
}

bool setCalibrated(bool calibrated) {
  if (calibrated) {
    EEPROM.write(CALIBRATED_ADDRESS, CALIBRATED_FLAG);
  } else {
    EEPROM.write(CALIBRATED_ADDRESS, 0);
  }
}
