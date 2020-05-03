# ESP32-BLE-Gamepad
Bluetooth LE Gamepad library for the ESP32 with Dual Joystick Support and designed for 7 button and 2 axes for simpler controllers. Trigger buttons and D-Hats can also easily be added.

This library allows you to make the ESP32 act as a Bluetooth Gamepad and control what it does. E.g. move axes and press buttons

## Features

 - [x] Button press (5 buttons)
 - [x] Button release (5 buttons)
 - [x] Axes movement (2 axes (x, y)) 
 - [x] Report optional battery level to host (basically works, but it doesn't show up in Android's status bar)
 - [x] Customize Bluetooth device name/manufacturer
 - [x] Compatible with Windows
 - [ ] Compatible with Android (Untested)
 - [ ] Compatible with Linux (Untested)
 - [ ] Compatible with MacOS X (Untested)
 - [ ] Compatible with iOS (Untested)

## Example

``` C++
/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 * 
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_5* 
 * 
 * bleGamepad.setAxes takes the following signed char parameters: 
 * (X, Y, IMU_X, IMU_Y);
 */
 
#include <BleGamepad.h> 

BleGamepad bleGamepad;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE stuff!");
  bleGamepad.begin();
}

void loop() {
  if(bleGamepad.isConnected()) {
    Serial.println("Press buttons 1 and 5. Move all axes to max.");
    bleGamepad.press(BUTTON_5);
    bleGamepad.press(BUTTON_1);
    bleGamepad.setAxes(127, 127, 127, 127);
    delay(500);

    Serial.println("Release button 7. Move all axes to min.");
    bleGamepad.release(BUTTON_7);
    bleGamepad.setAxes(-127, -127, -127, -127);
    delay(500);
  }
}
```

There is also Bluetooth specific information that you can use (optional):

Instead of `BleGamepad bleGamepad;` you can do `BleGamepad bleGamepad("Bluetooth Device Name", "Bluetooth Device Manufacturer", 100);`.
The third parameter is the initial battery level of your device. To adjust the battery level later on you can simply call e.g.  `bleMouse.setBatteryLevel(50)` (set battery level to 50%).
By default the battery level will be set to 100%, the device name will be `Joyduino ESP32` and the manufacturer will be `Attari Inc.`.


## Credits

Credits to [alijani1](https://github.com/alijani1) as his [Atari joystick library](https://github.com/alijani1/ESP32-BLE-Gamepad) library helped me figure out how to fine tune some aspects of the controller.

Credits to [lemmingDev](https://github.com/lemmingDev) as the BLE portion of this library is based on his [BLE gamepad](https://github.com/lemmingDev/ESP32-BLE-Gamepad).