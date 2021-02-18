# Memory layout


```
---------------------------------------------------------
| C | S |   X   |   Y   |   Z   |   X   |   Y   |   Z   |   ...
|   |   |         Side 1        |         Side 2        |
```


| Flag | Type | Description |
| ---- | ---- | ---- |
| C | byte |Determines whether the device is calibrated.|
| S | byte |Determines the count of the sides.|
| X | short |Determines the X coordinate of the side.|
| Y | short |Determines the Y coordinate of the side.|
| Z | short |Determines the Z coordinate of the side.|

Taking the Arduino Nano EEPROM memory size into account (**512 bytes**), the maximum count of sides available for configuration is **85**. As it is more than enough, the maximum count will be set to **25**.

