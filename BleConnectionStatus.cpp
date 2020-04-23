#include "BleConnectionStatus.h"

BleConnectionStatus::BleConnectionStatus(void) {
}

void BleConnectionStatus::onConnect(BLEServer* pServer)
{
  this->connected = true;
  BLE2902* desc = (BLE2902*)this->inputGamepad->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
  BLE2902* desc2 = (BLE2902*)this->inputGamepad2->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc2->setNotifications(true);

}

void BleConnectionStatus::onDisconnect(BLEServer* pServer)
{
  this->connected = false;
  BLE2902* desc = (BLE2902*)this->inputGamepad->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);
  BLE2902* desc2 = (BLE2902*)this->inputGamepad2->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc2->setNotifications(false);
}
