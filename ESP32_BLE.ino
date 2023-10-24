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


// position Characteristic
static BLEUUID PositionCharacteristicUUID("e9eb3f4e-5eda-11ee-8c99-0242ac120002");

// Current  Characteristic
static BLEUUID CurrentCharacteristicUUID("e9eb46b0-5eda-11ee-8c99-0242ac120002");

// Setpoint Characteristic
static BLEUUID SetpointCharacteristicUUID("e9eb482c-5eda-11ee-8c99-0242ac120002");

//Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;

//Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;
 
//Characteristicd that we want to read
static BLERemoteCharacteristic* Position_Charachteristic;
static BLERemoteCharacteristic* Current_Charachteristic;
static BLERemoteCharacteristic* Setpoint_Charachteristic;

//Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

char* PositionChar;
char* CurrentChar;
char* SetpointChar;

//Flags to check whether new temperature and humidity readings are available
boolean newPosition = false;
boolean newCurrent = false;
boolean newsetpoint = false;

//When the BLE Server sends a new Ring position value reading with the notify property
static void PositionNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store position value
  PositionChar = (char*)pData;
  newPosition= true;
}
//When the BLE Server sends a new Middle position value reading with the notify property
static void CurrentNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store Current value
  CurrentChar= (char*)pData;
  newCurrent = true;
}
//When the BLE Server sends a new Index position value reading with the notify property
static void SetpointNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //setpoint Index value
  SetpointChar = (char*)pData;
  newsetpoint = true;
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
  Position_Charachteristic = pRemoteService->getCharacteristic(PositionCharacteristicUUID);
  Current_Charachteristic = pRemoteService->getCharacteristic(CurrentCharacteristicUUID);
  Setpoint_Charachteristic = pRemoteService->getCharacteristic(SetpointCharacteristicUUID);

  if (Position_Charachteristic == nullptr ||Current_Charachteristic == nullptr||Setpoint_Charachteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");
 
  //Assign callback functions for the Characteristics
  Position_Charachteristic->registerForNotify(PositionNotifyCallback);
  Current_Charachteristic->registerForNotify(CurrentNotifyCallback);
  Setpoint_Charachteristic->registerForNotify(SetpointNotifyCallback);
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
  Serial.print(newPosition);
  Serial.println(PositionChar);
  Serial.print(newCurrent);
  Serial.println(CurrentChar);
  Serial.print(newsetpoint);
  Serial.println(SetpointChar);
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
      Position_Charachteristic->canNotify();
      Current_Charachteristic->canNotify();
      Setpoint_Charachteristic->canNotify();
      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  //if new temperature readings are available, print in the OLED
  if (newPosition|| newCurrent|| newsetpoint){
    printReadings();
    newPosition = false;
    newCurrent = false;
    newsetpoint = false;
  }
  delay(1000); // Delay a second between loops.
}