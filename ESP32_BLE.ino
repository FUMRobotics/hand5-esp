/*********
writen by MH.Taji
BLE Client
*********/

#include "BLEDevice.h"
#include "BLEClient.h"
#include "BLE2902.h"
#include <Arduino.h>

//BLE server name
#define bleServerName "Hand_ESP32"

struct
{
  float Pinky;
  float Ring;
  float Middle;
  float Index;
  float Thumb;
} HandFinger;

// See the following for generating UUIDs:
#define SERVICE_UUID "e9eb3b02-5eda-11ee-8c99-0242ac120002"

// Pinky Characteristic
static BLEUUID PinkyCharacteristicUUID("3777f9a2-69d3-11ee-8c99-0242ac120002");

// Ring Characteristic
static BLEUUID RingCharacteristicUUID("e9eb3f4e-5eda-11ee-8c99-0242ac120002");

// Middle Characteristic
static BLEUUID MiddleCharacteristicUUID("e9eb46b0-5eda-11ee-8c99-0242ac120002");

// Index Characteristic
static BLEUUID IndexCharacteristicUUID("e9eb482c-5eda-11ee-8c99-0242ac120002");

// Thumb Characteristic
static BLEUUID ThumbCharacteristicUUID("08c73b62-69d9-11ee-8c99-0242ac120002");

//Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;

//Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;
 
//Characteristicd that we want to read
static BLERemoteCharacteristic* Pinky_Finger_Charachteristic;
static BLERemoteCharacteristic* Ring_Finger_Charachteristic;
static BLERemoteCharacteristic* Middle_Finger_Charachteristic;
static BLERemoteCharacteristic* Index_Finger_Charachteristic;
static BLERemoteCharacteristic* Thumb_Finger_Charachteristic;

//Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

char* PinkyChar;
char* RingChar;
char* MiddleChar;
char* IndexChar;
char* ThumbChar;

//Flags to check whether new temperature and humidity readings are available
boolean newPinkyValue = false;
boolean newRingValue = false;
boolean newMiddleValue = false;
boolean newIndexValue = false;
boolean newThumbValue = false;

//When the BLE Server sends a new Pinky position value reading with the notify property
static void PinkyNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  PinkyChar = (char*)pData;
  newPinkyValue = true;
}
//When the BLE Server sends a new Ring position value reading with the notify property
static void RingNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store Ring value
  RingChar = (char*)pData;
  newRingValue = true;
}
//When the BLE Server sends a new Middle position value reading with the notify property
static void MiddleNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store Middle value
  MiddleChar= (char*)pData;
  newMiddleValue = true;
}
//When the BLE Server sends a new Index position value reading with the notify property
static void IndexNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store Index value
  IndexChar = (char*)pData;
  newIndexValue = true;
}
//When the BLE Server sends a new Thumb position value reading with the notify property
static void ThumbNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store Thumb value
  ThumbChar= (char*)pData;
  newThumbValue = true;
}

//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
   BLEClient* pClient = BLEDevice::createClient();
 
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");
 
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(SERVICE_UUID);
    return (false);
  }
 
  // Obtain a reference to the characteristics in the service of the remote BLE server.
  Pinky_Finger_Charachteristic = pRemoteService->getCharacteristic(PinkyCharacteristicUUID);
  Ring_Finger_Charachteristic = pRemoteService->getCharacteristic(RingCharacteristicUUID);
  Middle_Finger_Charachteristic = pRemoteService->getCharacteristic(MiddleCharacteristicUUID);
  Index_Finger_Charachteristic = pRemoteService->getCharacteristic(IndexCharacteristicUUID);
  Thumb_Finger_Charachteristic = pRemoteService->getCharacteristic(ThumbCharacteristicUUID);

  if (Pinky_Finger_Charachteristic == nullptr ||Ring_Finger_Charachteristic == nullptr|| Middle_Finger_Charachteristic == nullptr||Index_Finger_Charachteristic == nullptr||Thumb_Finger_Charachteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");
 
  //Assign callback functions for the Characteristics
  Pinky_Finger_Charachteristic->registerForNotify(PinkyNotifyCallback);
  Ring_Finger_Charachteristic->registerForNotify(RingNotifyCallback);
  Middle_Finger_Charachteristic->registerForNotify(MiddleNotifyCallback);
  Index_Finger_Charachteristic->registerForNotify(IndexNotifyCallback);
  Thumb_Finger_Charachteristic->registerForNotify(ThumbNotifyCallback);
  return true;
}

//Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) { //Check if the name of the advertiser matches
      advertisedDevice.getScan()->stop(); //Scan can be stopped, we found what we are looking for
      pServerAddress = new BLEAddress(advertisedDevice.getAddress()); //Address of advertiser is the one we need
      doConnect = true; //Set indicator, stating that we are ready to connect
      Serial.println("Device found. Connecting!");
    }
  }
};
 
//function that prints the latest sensor readings in the OLED display
void printReadings(){
  Serial.print(newPinkyValue);
  Serial.print("Pinky:");
  Serial.println(PinkyChar);
  Serial.print(newRingValue);
  Serial.print("Ring:");
  Serial.println(RingChar);
  Serial.print(newMiddleValue);
  Serial.print("Middle:");
  Serial.println(MiddleChar);
  Serial.print(newIndexValue);
  Serial.print("Index:");
  Serial.println(IndexChar);
  Serial.print(newThumbValue);
  Serial.print("Thumb:");
  Serial.println(ThumbChar);
}

void setup() {
  //Start serial communication
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");

  //Init BLE device
  BLEDevice::init("");
 
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(50);
}

void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      //Activate the Notify property of each Characteristic
      Pinky_Finger_Charachteristic->canNotify();
      // Pinky_Finger_Charachteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      Ring_Finger_Charachteristic->canNotify();
      // Ring_Finger_Charachteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      Middle_Finger_Charachteristic->canNotify();
      // Middle_Finger_Charachteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      Index_Finger_Charachteristic->canNotify();
      // Index_Finger_Charachteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      Thumb_Finger_Charachteristic->canNotify();
      // Thumb_Finger_Charachteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  //if new temperature readings are available, print in the OLED
  if (newPinkyValue || newRingValue || newMiddleValue || newIndexValue || newThumbValue){
    printReadings();
    newPinkyValue = false;
    newRingValue = false;
    newMiddleValue = false;
    newIndexValue = false;
    newThumbValue = false;
  }
  delay(1000); // Delay a second between loops.
}