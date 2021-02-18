#include <EEPROM.h>

const int MAX_SIDE_COUNT = 25;
const int INT_MAX_VALUE = 10000;

int* currentPosition = new int[3];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  currentPosition[0] = 0;
  currentPosition[1] = 0;
  currentPosition[2] = 0;
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
  Serial.println("Entered calibration mode.");

  bool finished = false;
  int** sides = new int*[MAX_SIDE_COUNT];
  int configuredSides = 0; 

  while (finished == false) {
    String input = readInput();
    
    if (input.indexOf("finish") != -1) {
      finished = true;
    } else {
      int* vector = parseVector(input);
      sides[configuredSides] = vector;
      configuredSides++;
  
      Serial.println("Configured side:");
      Serial.print("X=");
      Serial.print(vector[0]);
      Serial.print("Y=");
      Serial.print(vector[1]);
      Serial.print("Z=");
      Serial.print(vector[2]);
      Serial.println();
    }
  }

  Serial.println("Saving sides:");
  Serial.println(configuredSides);

  setCalibrated(true);
  setSides(sides, configuredSides);

  Serial.println("Calibration finished.");
}

void track() {
  bool calibrated = getCalibrated();
  if (calibrated) {
    int sidesCount = getSidesCount();
    int** sideData = getSides(sidesCount);

    int lowestDistance = INT_MAX_VALUE;
    int closestSide = -1;

    for (int i = 0; i < sidesCount; i++) {
      int newDistance = pow(sideData[i][0] - currentPosition[0], 2) 
        + pow(sideData[i][1] - currentPosition[1], 2) 
        + pow(sideData[i][2] - currentPosition[2], 2);

      if (newDistance < lowestDistance) {
        lowestDistance = newDistance;
        closestSide = i;
      }
    }

    Serial.println("Closest side:");
    Serial.println(closestSide);
  } else {
    Serial.println("The cube has not been calibrated.");
  }
}

String readInput() {
  while (Serial.available() <= 0) {
    delay(100);
  }

  return Serial.readString();
}

int* parseVector(String string) {
  String* elements = splitString(string);
  int* output = new int[3];
  output[0] = elements[0].toInt();
  output[1] = elements[1].toInt();
  output[2] = elements[2].toInt();

  return output;
}

String* splitString(String value) {
  String* output = new String[3];
  int valueLength = value.length();
  int currentElement = 0;
  int currentIndex = 0;

  for (int i = 0; i < valueLength; i++) {
    if (value[i] == ' ') {
      output[currentElement] = value.substring(currentIndex, i);
      currentElement++;
      currentIndex = i;
    }
  }

  return output;
}

// Memory handling

const int CALIBRATED_ADDRESS = 0;
const int CALIBRATED_FLAG = 178;

const int SIDE_COUNT_ADDRESS = 1;

bool getCalibrated() {
  int flag = EEPROM.read(CALIBRATED_ADDRESS);
  return flag == CALIBRATED_FLAG;
}

void setCalibrated(bool calibrated) {
  if (calibrated) {
    EEPROM.write(CALIBRATED_ADDRESS, CALIBRATED_FLAG);
  } else {
    EEPROM.write(CALIBRATED_ADDRESS, 0);
  }
}

int getSidesCount() {
  return EEPROM.read(SIDE_COUNT_ADDRESS);
}

int** getSides(int sideCount) {
  int** output = new int*[sideCount];

  for (int i = 0; i < sideCount; i++) {
    int* vector = new int[3];
    vector[0] = EEPROM.read(i * 6 + SIDE_COUNT_ADDRESS + 1);
    vector[1] = EEPROM.read(i * 6 + SIDE_COUNT_ADDRESS + 3);
    vector[2] = EEPROM.read(i * 6 + SIDE_COUNT_ADDRESS + 5);

    output[i] = vector;

    Serial.println("Read side:");
    Serial.print("X=");
    Serial.print(vector[0]);
    Serial.print("Y=");
    Serial.print(vector[1]);
    Serial.print("Z=");
    Serial.print(vector[2]);
    Serial.println();
  }

  return output;
}

void setSides(int** sideData, int count) {
  EEPROM.write(SIDE_COUNT_ADDRESS, count);

  for (int i = 0; i < count; i++) {
    int* vector = sideData[i];
    EEPROM.write(i * 6 + SIDE_COUNT_ADDRESS + 1, vector[0]);
    EEPROM.write(i * 6 + SIDE_COUNT_ADDRESS + 3, vector[1]);
    EEPROM.write(i * 6 + SIDE_COUNT_ADDRESS + 5, vector[2]);
  }
}
