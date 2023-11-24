/*********
writen by MH.Taji
BLE Server (PC)
*********/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

//BLE server name
#define bleServerName "Hand_ESP32"

bool new_current = 0;
bool new_Position = 0;
bool RXuart = 0;
String rxUART_Buf;
uint8_t counter_uart = 0;
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

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

bool deviceConnected = false;

// See the following for generating UUIDs:
#define SERVICE_UUID "e9eb3b02-5eda-11ee-8c99-0242ac120002"


// Position Characteristic and Descriptor
BLECharacteristic Position_Characteristics(
  "e9eb3f4e-5eda-11ee-8c99-0242ac120002",
  BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor Position_Descriptor(BLEUUID((uint16_t)0x2903));

// Current Characteristic and Descriptor
BLECharacteristic Current_Characteristics(
  "e9eb46b0-5eda-11ee-8c99-0242ac120002",
  BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor Current_Descriptor(BLEUUID((uint16_t)0x2904));

// setpoint Characteristic and Descriptor
BLECharacteristic Setpoint_Characteristics(
  "e9eb482c-5eda-11ee-8c99-0242ac120002",
  BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor Setpoint_Descriptor(BLEUUID((uint16_t)0x2905));


//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  };
};
class CurrentCallbacks : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic *pCharacteristic) {
    String CurrentStr;
    char *rxCurrentValue = (char *)pCharacteristic->getData();
    switch (rxCurrentValue[2]) {
      case 'P':
        CurrentStr = String(rxCurrentValue);
        HandCurrent.Pinky = CurrentStr.substring(6, CurrentStr.length() - 2).toInt();
        break;
      case 'R':
        CurrentStr = String(rxCurrentValue);
        HandCurrent.Ring = CurrentStr.substring(6, CurrentStr.length() - 2).toInt();
        break;
      case 'M':
        CurrentStr = String(rxCurrentValue);
        HandCurrent.Middle = CurrentStr.substring(6, CurrentStr.length() - 2).toInt();
        break;
      case 'I':
        CurrentStr = String(rxCurrentValue);
        HandCurrent.Index = CurrentStr.substring(6, CurrentStr.length() - 2).toInt();
        break;
      case 'T':
        CurrentStr = String(rxCurrentValue);
        HandCurrent.Thumb = CurrentStr.substring(6, CurrentStr.length() - 2).toInt();
        new_current = true;
        break;
    }
  }
};
class PositionCallbacks : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic *pCharacteristic) {
    String PositionStr;
    char *rxPositionValue = (char *)pCharacteristic->getData();
    switch (rxPositionValue[2]) {
      case 'P':
        PositionStr = String(rxPositionValue);
        HandPosition.Pinky = PositionStr.substring(6, PositionStr.length()).toFloat();
        break;
      case 'R':
        PositionStr = String(rxPositionValue);
        HandPosition.Ring = PositionStr.substring(6, PositionStr.length()).toFloat();
        break;
      case 'M':
        PositionStr = String(rxPositionValue);
        HandPosition.Middle = PositionStr.substring(6, PositionStr.length()).toFloat();
        break;
      case 'I':
        PositionStr = String(rxPositionValue);
        HandPosition.Index = PositionStr.substring(6, PositionStr.length()).toFloat();
        break;
      case 'T':
        PositionStr = String(rxPositionValue);
        HandPosition.Thumb = PositionStr.substring(6, PositionStr.length()).toFloat();
        new_Position = true;
        break;
    }
  }
};
void printReadings_Current() {
  log_e("*-*-*-*-*-*-*-Current-*-*-*-*-*-*-*");
  String strCurrent;
  strCurrent = "{PP:" + String(HandCurrent.Pinky) + "}";
  Serial.println(strCurrent);
  strCurrent = "{PR:" + String(HandCurrent.Ring) + "}";
  Serial.println(strCurrent);
  strCurrent = "{PM:" + String(HandCurrent.Middle) + "}";
  Serial.println(strCurrent);
  strCurrent = "{PI:" + String(HandCurrent.Index) + "}";
  Serial.println(strCurrent);
  strCurrent = "{PT:" + String(HandCurrent.Thumb) + "}";
  Serial.println(strCurrent);
}
void printReadings_Position() {
  log_e("-------Position-------");
  String strposition;
  strposition = "{CP:" + String(HandPosition.Pinky) + "}";
  Serial.println(strposition);
  strposition = "{CR:" + String(HandPosition.Ring) + "}";
  Serial.println(strposition);
  strposition = "{CM:" + String(HandPosition.Middle) + "}";
  Serial.println(strposition);
  strposition = "{CI:" + String(HandPosition.Index) + "}";
  Serial.println(strposition);
  strposition = "{CT:" + String(HandPosition.Thumb) + "}";
  Serial.println(strposition);
}
void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial2.begin(115200);

  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *HANDService = pServer->createService(SERVICE_UUID);

  //Position
  HANDService->addCharacteristic(&Position_Characteristics);
  Position_Descriptor.setValue("Position");
  Position_Characteristics.addDescriptor(&Position_Descriptor);
  Position_Characteristics.setCallbacks(new PositionCallbacks);
  //Current
  HANDService->addCharacteristic(&Current_Characteristics);
  Current_Descriptor.setValue("Current");
  Current_Characteristics.addDescriptor(&Current_Descriptor);
  Current_Characteristics.setCallbacks(new CurrentCallbacks);
  //setpoint
  HANDService->addCharacteristic(&Setpoint_Characteristics);
  Setpoint_Descriptor.setValue("Setpoint");
  Setpoint_Characteristics.addDescriptor(&Setpoint_Descriptor);

  // Start the service
  HANDService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  log_e("Waiting a client connection to notify...");
  log_e('\n');
}

void loop() {
  // if receive new current data from hand driver via BLE send by uart to matlab for Plot
  if (new_current) {
    printReadings_Current();
    new_current = false;
  }
  // if receive new position data from hand driver via BLE send by uart to matlab for Plot
  if (new_Position) {
    printReadings_Position();
    new_Position = false;
  }
  // if receive setpoint  data from matlab send for hand driver via BLE
  if (Serial2.available()) {
    rxUART_Buf = Serial2.readStringUntil('\n');
    // if set core debug level in error mode  see received data from matlab in serial monitor
    log_e(rxUART_Buf);
    //parse received setpoint from matlab for each finger
    HandSetPoint.Pinky = rxUART_Buf.substring(rxUART_Buf.indexOf("P:") + 2, rxUART_Buf.indexOf("R")).toFloat();
    HandSetPoint.Ring = rxUART_Buf.substring(rxUART_Buf.indexOf("R:") + 2, rxUART_Buf.indexOf("M")).toFloat();
    HandSetPoint.Middle = rxUART_Buf.substring(rxUART_Buf.indexOf("M:") + 2, rxUART_Buf.indexOf("I")).toFloat();
    HandSetPoint.Index = rxUART_Buf.substring(rxUART_Buf.indexOf("I:") + 2, rxUART_Buf.indexOf("T")).toFloat();
    HandSetPoint.Thumb = rxUART_Buf.substring(rxUART_Buf.indexOf("T:") + 2, rxUART_Buf.indexOf("}")).toFloat();
    //check if connected client send setpoint via BLE
    if (deviceConnected) {
      String Fingers_Value;
      //Set Setpoint  Characteristics value and notify connected client
      //set Pinky value
      log_e("-------------");
      Fingers_Value = "{\"P\":\"" + String(HandSetPoint.Pinky) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e(Fingers_Value);
      //set Ring value
      Fingers_Value = "{\"R\":\"" + String(HandSetPoint.Ring) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e(Fingers_Value);
      //set Middle value
      Fingers_Value = "{\"M\":\"" + String(HandSetPoint.Middle) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e(Fingers_Value);
      //set Index value
      Fingers_Value = "{\"I\":\"" + String(HandSetPoint.Index) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e(Fingers_Value);
      //set Thumb value
      Fingers_Value = "{\"T\":\"" + String(HandSetPoint.Thumb) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e(Fingers_Value);
      log_e("-------------");
    }
  }
}