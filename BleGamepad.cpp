#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#include "BleConnectionStatus.h"
#include "BleGamepad.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "BLEDevice";
#endif

static const uint8_t _hidReportDescriptor[] = {
  USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x05, // USAGE (Gamepad)
  COLLECTION(1),       0x01, // COLLECTION (Application)
  USAGE(1),            0x01, //   USAGE (Pointer)
  COLLECTION(1),       0x00, //   COLLECTION (Physical)
  REPORT_ID(1),        0x01, //     REPORT_ID (1)
  // ------------------------------------------------- Buttons (1 to 7)
  // Here we assign the bits needed to handle our 7 buttons
  USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
  USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
  USAGE_MAXIMUM(1),    0x07, //     USAGE_MAXIMUM (Button 7)
  LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x07, //     REPORT_COUNT (7)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;7 button bits
  // ------------------------------------------------- Padding
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1) ; the padding
  HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;1 bit padding
  // ------------------------------------------------- X/Y position
  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x30, //     USAGE (X)
  USAGE(1),            0x31, //     USAGE (Y)
  /**Traditionally, Z represents the right stick’s X axis, Rx is used to represent
   *  the right stick’s Y axis. It's stupid but it's is how most existing USB game pads work.
   *  You can change it to that, or be a rebel like me and keep this configuration. The choice is yours Neo. **/
  USAGE(1),            0x33, //     USAGE (Rx -- represents IMU_X)
  USAGE(1),            0x34, //     USAGE (Ry -- represents IMU_Y)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x04, //     REPORT_COUNT (4)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;4 bytes (X,Y,Z,rZ)

  END_COLLECTION(0),         //   END_COLLECTION
  END_COLLECTION(0),         // END_COLLECTION


  
  // The commented code can be uncommented to allow 2 controllers to connect at once!

  // USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
  // USAGE(1),            0x05, // USAGE (Gamepad)
  // COLLECTION(1),       0x01, // COLLECTION (Application)
  // USAGE(1),            0x01, //   USAGE (Pointer)
  // COLLECTION(1),       0x00, //   COLLECTION (Physical)
  // REPORT_ID(1),        0x02, //     REPORT_ID (2)
  // // ------------------------------------------------- Buttons (1 to 7)
  // USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
  // USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
  // USAGE_MAXIMUM(1),    0x07, //     USAGE_MAXIMUM (Button 7)
  // LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
  // LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
  // REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  // REPORT_COUNT(1),     0x07, //     REPORT_COUNT (7)
  // HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;7 button bits
  // // ------------------------------------------------- Padding
  // REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  // REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  // HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;1 bit padding
  // // ------------------------------------------------- X/Y position
  // USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  // USAGE(1),            0x30, //     USAGE (X)
  // USAGE(1),            0x31, //     USAGE (Y)
  // LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  // LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  // REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  // REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
  // HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;2 bytes (X,Y)
  // END_COLLECTION(0),         //   END_COLLECTION
  // END_COLLECTION(0)          // END_COLLECTION
};

BleGamepad::BleGamepad(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) :
  _buttons(0),
  hid(0)
{
  this->deviceName = deviceName;
  this->deviceManufacturer = deviceManufacturer;
  this->batteryLevel = batteryLevel;
  this->connectionStatus = new BleConnectionStatus();
}

void BleGamepad::begin(void)
{
  xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleGamepad::end(void)
{
}

void BleGamepad::setAxes(signed char joyStick_x, signed char joyStick_y, signed char IMU_x, signed char IMU_y)
{
  if (this->isConnected())
  {
    uint8_t m[5];
    m[0] = _buttons;
    m[1] = joyStick_x;
    m[2] = joyStick_y;
    m[3] = IMU_x;
    m[4] = IMU_y;
    this->inputGamepad->setValue(m, sizeof(m));
    this->inputGamepad->notify();
  }
}

  // The commented code can be uncommented to allow 2 controllers to connect at once!

// void BleGamepad::setStick2(signed char x, signed char y, uint8_t b )
// {
//   if (this->isConnected())
//   {
//     uint8_t m[3];
//     m[0] = b;
//     m[1] = x;
//     m[2] = y;
//     this->inputGamepad2->setValue(m, sizeof(m));
//     this->inputGamepad2->notify();
//   }
// }

void BleGamepad::setStick(signed char x, signed char y, uint8_t b )
{
  if (this->isConnected())
  {
    uint8_t m[3];
    m[0] = b;
    m[1] = x;
    m[2] = y;
    this->inputGamepad->setValue(m, sizeof(m));
    this->inputGamepad->notify();
  }
}




void BleGamepad::buttons(uint16_t b)
{
  if (b != _buttons)
  {
    _buttons = b;
    setAxes(0, 0, 0, 0);
  }
}

void BleGamepad::press(uint16_t b)
{
  buttons(_buttons | b);
}

void BleGamepad::release(uint16_t b)
{
  buttons(_buttons & ~b);
}

bool BleGamepad::isPressed(uint16_t b)
{
  if ((b & _buttons) > 0)
    return true;
  return false;
}

bool BleGamepad::isConnected(void) {
  return this->connectionStatus->connected;
}

void BleGamepad::setBatteryLevel(uint8_t level) {
  this->batteryLevel = level;
  if (hid != 0)
    this->hid->setBatteryLevel(this->batteryLevel);
}

void BleGamepad::taskServer(void* pvParameter) {
  BleGamepad* BleGamepadInstance = (BleGamepad *) pvParameter; //static_cast<BleGamepad *>(pvParameter);
  BLEDevice::init(BleGamepadInstance->deviceName);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(BleGamepadInstance->connectionStatus);

  // The commented code can be uncommented to allow 2 controllers to connect at once! Make sure you uncomment the other
  // references to inputGamepad2 throughout the library!
  
  BleGamepadInstance->hid = new BLEHIDDevice(pServer);
  BleGamepadInstance->inputGamepad = BleGamepadInstance->hid->inputReport(1); // <-- input REPORTID from report map
  // BleGamepadInstance->inputGamepad2 = BleGamepadInstance->hid->inputReport(2); // <-- input REPORTID from report map
  BleGamepadInstance->connectionStatus->inputGamepad = BleGamepadInstance->inputGamepad;
  // BleGamepadInstance->connectionStatus->inputGamepad2 = BleGamepadInstance->inputGamepad2;

  BleGamepadInstance->hid->manufacturer()->setValue(BleGamepadInstance->deviceManufacturer);
  BleGamepadInstance->hid->pnp(0x01, 0x8282, 0x0132, 0x0106);
  BleGamepadInstance->hid->hidInfo(0x00,0x01);

  BLESecurity *pSecurity = new BLESecurity();

  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  BleGamepadInstance->hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  BleGamepadInstance->hid->startServices();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->addServiceUUID(BleGamepadInstance->hid->hidService()->getUUID());
  pAdvertising->start();
  BleGamepadInstance->hid->setBatteryLevel(BleGamepadInstance->batteryLevel);

  ESP_LOGD(LOG_TAG, "Advertising started!");
  vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}
