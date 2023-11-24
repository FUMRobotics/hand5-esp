/*********
writen by MH.Taji
BLE Client (hand)
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
} HandSetPoint;
struct
{
  float Pinky;
  float Ring;
  float Middle;
  float Index;
  float Thumb;
} HandPosition;
struct
{
  uint16_t Pinky;
  uint16_t Ring;
  uint16_t Middle;
  uint16_t Index;
  uint16_t Thumb;
} HandCurrent;

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
static BLEAddress* pServerAddress;

//Characteristicd that we want to read
static BLERemoteCharacteristic* Position_Charachteristic;
static BLERemoteCharacteristic* Current_Charachteristic;
static BLERemoteCharacteristic* Setpoint_Charachteristic;

//Activate notify
const uint8_t notificationOn[] = { 0x1, 0x0 };
const uint8_t notificationOff[] = { 0x0, 0x0 };

char* PositionChar;
char* CurrentChar;
char* SetpointChar;

//Flags to check whether new temperature and humidity readings are available
boolean newPosition = false;
boolean newCurrent = false;
boolean newsetpoint = false;

//When the BLE Server sends a new Index position value reading with the notify property
static void SetpointNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                                   uint8_t* pData, size_t length, bool isNotify) {
  //setpoint Index value
  String strFinger;
  SetpointChar = (char*)pData;
  switch (SetpointChar[2]) {
    case 'P':
      strFinger = String(SetpointChar);
      HandSetPoint.Pinky = strFinger.substring(6, length - 2).toFloat();
      break;
    case 'R':
      strFinger = String(SetpointChar);
      HandSetPoint.Ring = strFinger.substring(6, length - 2).toFloat();
      break;
    case 'M':
      strFinger = String(SetpointChar);
      HandSetPoint.Middle = strFinger.substring(6, length - 2).toFloat();
      break;
    case 'I':
      strFinger = String(SetpointChar);
      HandSetPoint.Index = strFinger.substring(6, length - 2).toFloat();
      break;
    case 'T':
      strFinger = String(SetpointChar);
      HandSetPoint.Thumb = strFinger.substring(6, length - 2).toFloat();
      break;
  }
  newsetpoint = true;
}

//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
  BLEClient* pClient = BLEDevice::createClient();

  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  log_e(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    log_e("Failed to find our service UUID: ");
    log_e(SERVICE_UUID);
    return (false);
  }

  // Obtain a reference to the characteristics in the service of the remote BLE server.
  Position_Charachteristic = pRemoteService->getCharacteristic(PositionCharacteristicUUID);
  Current_Charachteristic = pRemoteService->getCharacteristic(CurrentCharacteristicUUID);
  Setpoint_Charachteristic = pRemoteService->getCharacteristic(SetpointCharacteristicUUID);

  if (Position_Charachteristic == nullptr || Current_Charachteristic == nullptr || Setpoint_Charachteristic == nullptr) {
    log_e("Failed to find our characteristic UUID");
    return false;
  }
  log_e(" - Found our characteristics");

  //Assign callback functions for the Characteristics
  // Position_Charachteristic->registerForNotify(PositionNotifyCallback);
  // Current_Charachteristic->registerForNotify(CurrentNotifyCallback);
  Setpoint_Charachteristic->registerForNotify(SetpointNotifyCallback);
  return true;
}

//Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) {                 //Check if the name of the advertiser matches
      advertisedDevice.getScan()->stop();                              //Scan can be stopped, we found what we are looking for
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());  //Address of advertiser is the one we need
      doConnect = true;                                                //Set indicator, stating that we are ready to connect
      log_e("Device found. Connecting!");
    }
  }
};

//function that prints the received setpoint
void printReadings() {
  String setpoint_str;
  setpoint_str = "{P:" + String(HandSetPoint.Pinky) + "R:" + String(HandSetPoint.Ring) + "M:" + String(HandSetPoint.Middle) + "I:" + String(HandSetPoint.Index) + "T:" + String(HandSetPoint.Thumb) + "}";
  Serial.println(setpoint_str);
}

void setup() {
  //Start serial communication
  Serial.begin(115200);
  Serial2.begin(115200);
  log_e("Starting Arduino BLE Client application...");

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
      log_e("We are now connected to the BLE Server.");
      //Activate the Notify property of each Characteristic
      Setpoint_Charachteristic->canNotify();
      connected = true;
    } else {
      log_e("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  // if new newsetpoint readings are available
  if (newsetpoint) {
    printReadings();
    newsetpoint = false;
  }
  if (Serial2.available()) {
    //read data from uart(hand driver)
    String uartRX = Serial2.readStringUntil('\n');
    //check if connect to server then pars received data from uart(hand driver)
    if (connected) {
      //detect current data or position data
      if (uartRX.startsWith("{CP:")) {
        // parse current data for each finger
        HandCurrent.Pinky = uartRX.substring(uartRX.indexOf("P:") + 2, uartRX.indexOf("CR")).toInt();
        HandCurrent.Ring = uartRX.substring(uartRX.indexOf("R:") + 2, uartRX.indexOf("CM")).toInt();
        HandCurrent.Middle = uartRX.substring(uartRX.indexOf("M:") + 2, uartRX.indexOf("CI")).toInt();
        HandCurrent.Index = uartRX.substring(uartRX.indexOf("I:") + 2, uartRX.indexOf("CT")).toInt();
        HandCurrent.Thumb = uartRX.substring(uartRX.indexOf("T:") + 2, uartRX.indexOf("}")).toInt();
        // HandCurrent.Index++;
        // HandCurrent.Pinky++;
        // HandCurrent.Middle++;
        // HandCurrent.Thumb++;
        // HandCurrent.Ring++;
        //******************************************* send current via BLE ******************************************
        String CurrentValue;
        //set Pinky value
        log_e("------------- | Current |---------------");
        CurrentValue = "{\"P\":\"" + String(HandCurrent.Pinky) + "\"}";
        Current_Charachteristic->writeValue(CurrentValue.c_str(), CurrentValue.length());
        log_e(CurrentValue);
        //set Ring value
        CurrentValue = "{\"R\":\"" + String(HandCurrent.Ring) + "\"}";
        Current_Charachteristic->writeValue(CurrentValue.c_str(), CurrentValue.length());
        log_e(CurrentValue);
        //set Middle value
        CurrentValue = "{\"M\":\"" + String(HandCurrent.Middle) + "\"}";
        Current_Charachteristic->writeValue(CurrentValue.c_str(), CurrentValue.length());
        log_e(CurrentValue);
        //set Index value
        CurrentValue = "{\"I\":\"" + String(HandCurrent.Index) + "\"}";
        Current_Charachteristic->writeValue(CurrentValue.c_str(), CurrentValue.length());
        log_e(CurrentValue);
        //set Thumb value
        CurrentValue = "{\"T\":\"" + String(HandCurrent.Thumb) + "\"}";
        Current_Charachteristic->writeValue(CurrentValue.c_str(), CurrentValue.length());
        log_e(CurrentValue);
        log_e("***");
      } else {
        // parse position data for each finger
        HandPosition.Pinky = uartRX.substring(uartRX.indexOf("P:") + 2, uartRX.indexOf("PR")).toInt();
        HandPosition.Ring = uartRX.substring(uartRX.indexOf("R:") + 2, uartRX.indexOf("PM")).toInt();
        HandPosition.Middle = uartRX.substring(uartRX.indexOf("M:") + 2, uartRX.indexOf("PI")).toInt();
        HandPosition.Index = uartRX.substring(uartRX.indexOf("I:") + 2, uartRX.indexOf("PT")).toInt();
        HandPosition.Thumb = uartRX.substring(uartRX.indexOf("T:") + 2, uartRX.indexOf("}")).toInt();
        // HandPosition.Index += 0.01;
        // HandPosition.Pinky += 0.01;
        // HandPosition.Middle += 0.01;
        // HandPosition.Thumb += 0.01;
        // HandPosition.Ring += 0.01;
        //******************************************* send position via BLE ******************************************
        String PositionValue;
        //set Pinky value
        log_e("------------- | Position |---------------");
        PositionValue = "{\"P\":\"" + String(HandPosition.Pinky) + "\"}";
        Position_Charachteristic->writeValue(PositionValue.c_str(), PositionValue.length());
        log_e(PositionValue);
        //set Ring value
        PositionValue = "{\"R\":\"" + String(HandPosition.Ring) + "\"}";
        Position_Charachteristic->writeValue(PositionValue.c_str(), PositionValue.length());
        log_e(PositionValue);
        //set Middle value
        PositionValue = "{\"M\":\"" + String(HandPosition.Middle) + "\"}";
        Position_Charachteristic->writeValue(PositionValue.c_str(), PositionValue.length());
        log_e(PositionValue);
        //set Index value
        PositionValue = "{\"I\":\"" + String(HandPosition.Index) + "\"}";
        Position_Charachteristic->writeValue(PositionValue.c_str(), PositionValue.length());
        log_e(PositionValue);
        //set Thumb value
        PositionValue = "{\"T\":\"" + String(HandPosition.Thumb) + "\"}";
        Position_Charachteristic->writeValue(PositionValue.c_str(), PositionValue.length());
        log_e(PositionValue);
        log_e("***");
      }
    }
  }
}