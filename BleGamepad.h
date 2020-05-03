#ifndef ESP32_BLE_GAMEPAD_H
#define ESP32_BLE_GAMEPAD_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "BleConnectionStatus.h"
#include "BLEHIDDevice.h"
#include "BLECharacteristic.h"

#define BUTTON_1 	1
#define BUTTON_2 	2
#define BUTTON_3 	4
#define BUTTON_4 	8
#define BUTTON_5 	16
#define BUTTON_6 	32
#define BUTTON_7 	64
#define BUTTON_8 	128


class BleGamepad {
private:
  uint16_t _buttons;
  BleConnectionStatus* connectionStatus;
  BLEHIDDevice* hid;
  BLECharacteristic* inputGamepad;
  // BLECharacteristic* inputGamepad2;
  void buttons(uint16_t b);
  void rawAction(uint8_t msg[], char msgSize);
  static void taskServer(void* pvParameter);
public:
  BleGamepad(std::string deviceName = "Joyduino ESP32", std::string deviceManufacturer = "Attari Inc.", uint8_t batteryLevel = 100);
  void begin(void);
  void end(void);
  void setAxes(signed char x, signed char y);
  void setStick(signed char x, signed char y, uint8_t b);
  // void setStick2(signed char x, signed char y, uint8_t b);
  void press(uint16_t b = BUTTON_1);   // press BUTTON_1 by default
  void release(uint16_t b = BUTTON_1); // release BUTTON_1 by default
  bool isPressed(uint16_t b = BUTTON_1); // check BUTTON_1 by default
  bool isConnected(void);
  void setBatteryLevel(uint8_t level);
  uint8_t batteryLevel;
  std::string deviceManufacturer;
  std::string deviceName;
};

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_GAMEPAD_H
