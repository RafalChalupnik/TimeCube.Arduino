#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available() > 0) {
    String receivedString = Serial.readString();
    char buffer[receivedString.length()];
    receivedString.toCharArray(buffer, receivedString.length());
    int value = atoi(buffer);
    Serial.println("Writing to EEPROM:");
    Serial.println(value);
    EEPROM.write(0, value);
  } else {
    int value = EEPROM.read(0);
    Serial.println("Reading from EEPROM:");
    Serial.println(value);
  }

  delay(1000);
}
