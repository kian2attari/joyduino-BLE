/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 * 
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_5* 
 * 
 * 
 * bleGamepad.setAxes takes the following signed char parameters: 
 * (X, Y);
 */
 
#include <BleGamepad.h> 

BleGamepad bleGamepad;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleGamepad.begin();
}

void loop() {
  if(bleGamepad.isConnected()) {
    Serial.println("Press buttons 1 and 5. Move all axes to max.");
    bleGamepad.press(BUTTON_5);
    bleGamepad.press(BUTTON_1);
    bleGamepad.setAxes(127, 127);
    delay(500);

    Serial.println("Release button 5. Move all axes to min.");
    bleGamepad.release(BUTTON_5);
    bleGamepad.setAxes(-127, -127);
    delay(500);
  }
}