# IR and RF controller over WiFi

## Requirements (software)
- Arduino IDE (https://www.arduino.cc/en/Main/Software)
- WiFiNINA (https://www.arduino.cc/en/Reference/WiFiNINA)
- IRLib2 (https://github.com/cyborg5/IRLib2)
- rc-switch (https://github.com/sui77/rc-switch)

## Requirements (hardware)
- Arduino MKR 1010 WiFi (https://store.arduino.cc/mkr-wifi-1010)
- Infrared LED emitter
- 82 Ohms resistor
- PWM-compatible MOSFET, for example, PWM module with IRF520
- Radio frequency emitter module

## Installation
1. Download and install Arduino IDE
1. Clone this repository
2. Load IRLib2 into Arduino Libraries
    - Guide https://www.arduino.cc/en/guide/libraries
3. Patch IRLib2 according to what is inside *patch* folder (*diff* format)
4. Download rc-switch and WiFiNINA using the Arduino Library Manager
5. Create a file named *secrets.h* using the same format as shown in *secrets.h.example*
4. Compile and upload

## Hardware setup
- Pin 0 = IR emitter
- Pin 4 = RF emitter

## Manual
Comming soon...

## Licensing
This project is under [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)
