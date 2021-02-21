#include <EEPROM.h>

const int INT_MAX_VALUE = 10000;
const int INTERVAL = 1000;
const int MAX_SIDE_COUNT = 25;

const String CALIBRATE_COMMAND = "calibrate";
const String FINISH_COMMAND = "finish";

const String CLOSEST_SIDE_MESSAGE = "ClosestSide: ";
const String NOT_CALIBRATED_MESSAGE = "NotCalibrated";

bool _calibrated;

int _sidesCount;
int** _sidesData;
int* _currentPosition;

void setup() {
  Serial.begin(9600);

  _calibrated = getCalibrated();

  if (_calibrated) {
    _sidesCount = getSidesCount();
    _sidesData = getSides(_sidesCount);
  }
  
  _currentPosition = new int[3];
  _currentPosition[0] = 0;
  _currentPosition[1] = 0;
  _currentPosition[2] = 0;
}

void loop() {
  if (requestedCalibration()) {
    calibrate();
  } else {
    track();
  }

  delay(INTERVAL);
}

bool requestedCalibration() {
  if (Serial.available() <= 0) {
    return false;
  }

  String receivedString = Serial.readString();
  return contains(receivedString, CALIBRATE_COMMAND);
}

void calibrate() {
  log("Entered calibration mode.");

  bool finished = false;
  int** sides = new int*[MAX_SIDE_COUNT];
  int sidesCount = 0;

  while (finished == false) {
    String input = readInput();

    if (contains(input, FINISH_COMMAND)) {
      finished = true;
    } else {
      int* vector = parseVector(input);
      sides[sidesCount] = vector;
      sidesCount++;
  
      log("Configured side:");
      log(vector);
    }
  }

  _calibrated = true;
  setCalibrated(true);

  setSides(sides, sidesCount);

  delete _sidesData;
  _sidesData = sides;
  _sidesCount = sidesCount;
  Serial.println("Calibration finished.");
}

void track() {
  if (_calibrated == false) {
    Serial.println(NOT_CALIBRATED_MESSAGE);
    return;
  }

  readCurrentPosition();
  
  int lowestDistance = INT_MAX_VALUE;
  int closestSide = -1;

  int newDistance;

  for (int side = 0; side < _sidesCount; side++) {
    newDistance = calculateDistance(side);

    if (newDistance < lowestDistance) {
      lowestDistance = newDistance;
      closestSide = side;
    }
  }

  Serial.print(CLOSEST_SIDE_MESSAGE);
  Serial.println(closestSide);
}

void readCurrentPosition() {
  // TODO: Return data from the accelerometer/gyroscope
}

int calculateDistance(int side) {
  return pow(_sidesData[side][0] - _currentPosition[0], 2) 
    + pow(_sidesData[side][1] - _currentPosition[1], 2) 
    + pow(_sidesData[side][2] - _currentPosition[2], 2);
}

// Memory handling

const int CALIBRATED_ADDRESS = 0;
const byte CALIBRATED_FLAG = 178;

const int SIDE_COUNT_ADDRESS = 1;

bool getCalibrated() {
  return eeprom_readByte(CALIBRATED_ADDRESS) == CALIBRATED_FLAG;
}

void setCalibrated(bool calibrated) {
  if (calibrated) {
    eeprom_writeByte(CALIBRATED_ADDRESS, CALIBRATED_FLAG);
  } else {
    eeprom_writeByte(CALIBRATED_ADDRESS, 0);
  }
}

byte getSidesCount() {
  return eeprom_readByte(SIDE_COUNT_ADDRESS);
}

int** getSides(int sideCount) {
  int** output = new int*[sideCount];

  for (int i = 0; i < sideCount; i++) {
    int* vector = new int[3];
    int sideAddress = getSideAddress(i);

    vector[0] = eeprom_readInt(sideAddress);
    vector[1] = eeprom_readInt(sideAddress + 2);
    vector[2] = eeprom_readInt(sideAddress + 4);

    output[i] = vector;

    log("Read side:");
    log(vector);
  }

  return output;
}

void setSides(int** sideData, byte count) {
  eeprom_writeByte(SIDE_COUNT_ADDRESS, count);

  for (int i = 0; i < count; i++) {
    int* vector = sideData[i];
    int sideAddress = getSideAddress(i);
    
    eeprom_writeInt(sideAddress, vector[0]);
    eeprom_writeInt(sideAddress + 2, vector[1]);
    eeprom_writeInt(sideAddress + 4, vector[2]);
  }
}

int getSideAddress(int side) {
  return side * 6 + SIDE_COUNT_ADDRESS + 1;
}

// EEPROM

byte eeprom_readByte(int address) {
  return EEPROM.read(address);
}

int eeprom_readInt(int address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);

  return (byte1 << 8) + byte2;
}

void eeprom_writeByte(int address, byte value) {
  EEPROM.write(address, value);
}

void eeprom_writeInt(int address, int value) {
  EEPROM.write(address, value >> 8);
  EEPROM.write(address + 1, value & 0xFF);
}

// Utilities

bool contains(String value, String check) {
  return value.indexOf(check) != -1;
}

void log(String message) {
  Serial.println(message);
}

void log(int* vector) {
  for (int i = 0; i < 3; i++) {
    Serial.print(vector[i]);
    Serial.print(";");
  }

  Serial.println();
}

int* parseVector(String string) {
  String* elements = splitString(string, " ");
  int* output = new int[3];

  for (int i = 0; i < 3; i++) {
    log("Parsing:");
    log(elements[i]);
    output[i] = elements[i].toInt();
  }

  delete elements;
  return output;
}

String readInput() {
  while (Serial.available() <= 0) {
    delay(100);
  }

  return Serial.readString();;
}

String* splitString(String value, String delimiter) {
  String* output = new String[3];
  String remainingString = value;
  int elements = 0;
  int delimiterIndex;

  while (remainingString != "") {
    delimiterIndex = remainingString.indexOf(delimiter);
    if (delimiterIndex != -1) {
      output[elements] = remainingString.substring(0, delimiterIndex);
      remainingString = remainingString.substring(delimiterIndex + 1);
    } else {
      output[elements] = remainingString;
      remainingString = "";
    }

    elements++;
  }

  return output;
}
