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
#define debug
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

enum {
  speed,
  position
} ControlMode;
// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

bool deviceConnected = false;
bool send_test_data = 0;
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
  #ifndef debug
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
  #endif
  #ifdef debug
  log_e("*-*-*-*-*-*-*-Current-*-*-*-*-*-*-*");
  String strCurrent;
  strCurrent = "{PP:" + String(HandCurrent.Pinky) + "}";
  Serial2.println(strCurrent);
  strCurrent = "{PR:" + String(HandCurrent.Ring) + "}";
  Serial2.println(strCurrent);
  strCurrent = "{PM:" + String(HandCurrent.Middle) + "}";
  Serial2.println(strCurrent);
  strCurrent = "{PI:" + String(HandCurrent.Index) + "}";
  Serial2.println(strCurrent);
  strCurrent = "{PT:" + String(HandCurrent.Thumb) + "}";
  Serial2.println(strCurrent);
  #endif
}
void printReadings_Position() {
  #ifndef debug
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
  #endif
  #ifdef debug
  log_e("-------Position-------");
  String strposition;
  strposition = "{CP:" + String(HandPosition.Pinky) + "}";
  Serial2.println(strposition);
  strposition = "{CR:" + String(HandPosition.Ring) + "}";
  Serial2.println(strposition);
  strposition = "{CM:" + String(HandPosition.Middle) + "}";
  Serial2.println(strposition);
  strposition = "{CI:" + String(HandPosition.Index) + "}";
  Serial2.println(strposition);
  strposition = "{CT:" + String(HandPosition.Thumb) + "}";
  Serial2.println(strposition);
  #endif
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
  log_e("\n");
}

void loop() {
  // if receive new current & new position data from hand driver via BLE send by uart to matlab for Plot
  if (new_current && new_Position) {
    printReadings_Current();
    printReadings_Position();
    new_current = false;
    new_Position = false;
  }
  // if receive setpoint  data from matlab send for hand driver via BLE
  if (Serial2.available()) {
    rxUART_Buf = Serial2.readStringUntil('\n');
    // if set core debug level in error mode  see received data from matlab in serial monitor
    log_e("[%s]", rxUART_Buf);
    //parse received setpoint from matlab for each finger
    if (rxUART_Buf.startsWith("{SP")) {
      HandSetPoint.Pinky = rxUART_Buf.substring(rxUART_Buf.indexOf("P:") + 2, rxUART_Buf.indexOf("SR")).toFloat();
      HandSetPoint.Ring = rxUART_Buf.substring(rxUART_Buf.indexOf("R:") + 2, rxUART_Buf.indexOf("SM")).toFloat();
      HandSetPoint.Middle = rxUART_Buf.substring(rxUART_Buf.indexOf("M:") + 2, rxUART_Buf.indexOf("SI")).toFloat();
      HandSetPoint.Index = rxUART_Buf.substring(rxUART_Buf.indexOf("I:") + 2, rxUART_Buf.indexOf("ST")).toFloat();
      HandSetPoint.Thumb = rxUART_Buf.substring(rxUART_Buf.indexOf("T:") + 2, rxUART_Buf.indexOf("}")).toFloat();
      ControlMode = speed;
      log_e("speed mode : P%fR%fM%fI%fT%f", HandSetPoint.Pinky, HandSetPoint.Ring, HandSetPoint.Middle, HandSetPoint.Index, HandSetPoint.Thumb);
#ifdef debug
      //just for test uart communication between esp and matlab
      new_current = 1;
      new_Position = 1;
      HandPosition.Pinky=HandSetPoint.Pinky+5;
      HandPosition.Ring=HandSetPoint.Ring+5;
      HandPosition.Middle=HandSetPoint.Middle+5;
      HandPosition.Index=HandSetPoint.Index+5;
      HandPosition.Thumb=HandSetPoint.Thumb+5;
      HandCurrent.Pinky=HandSetPoint.Pinky+10;
      HandCurrent.Ring=HandSetPoint.Ring+10;
      HandCurrent.Middle=HandSetPoint.Middle+10;
      HandCurrent.Index=HandSetPoint.Index+10;
      HandCurrent.Thumb=HandSetPoint.Thumb+10;
#endif
    } else if (rxUART_Buf.startsWith("{PP")) {
      HandSetPoint.Pinky = rxUART_Buf.substring(rxUART_Buf.indexOf("P:") + 2, rxUART_Buf.indexOf("PR")).toFloat();
      HandSetPoint.Ring = rxUART_Buf.substring(rxUART_Buf.indexOf("R:") + 2, rxUART_Buf.indexOf("PM")).toFloat();
      HandSetPoint.Middle = rxUART_Buf.substring(rxUART_Buf.indexOf("M:") + 2, rxUART_Buf.indexOf("PI")).toFloat();
      HandSetPoint.Index = rxUART_Buf.substring(rxUART_Buf.indexOf("I:") + 2, rxUART_Buf.indexOf("PT")).toFloat();
      HandSetPoint.Thumb = rxUART_Buf.substring(rxUART_Buf.indexOf("T:") + 2, rxUART_Buf.indexOf("}")).toFloat();
      ControlMode = position;
      log_e("position mode : P%fR%fM%fI%fT%f", HandSetPoint.Pinky, HandSetPoint.Ring, HandSetPoint.Middle, HandSetPoint.Index, HandSetPoint.Thumb);
#ifdef debug
      //just for test uart communication between esp and matlab
      new_current = 1;
      new_Position = 1;
      HandPosition.Pinky=HandSetPoint.Pinky+5;
      HandPosition.Ring=HandSetPoint.Ring+5;
      HandPosition.Middle=HandSetPoint.Middle+5;
      HandPosition.Index=HandSetPoint.Index+5;
      HandPosition.Thumb=HandSetPoint.Thumb+5;
      HandCurrent.Pinky=HandSetPoint.Pinky+10;
      HandCurrent.Ring=HandSetPoint.Ring+10;
      HandCurrent.Middle=HandSetPoint.Middle+10;
      HandCurrent.Index=HandSetPoint.Index+10;
      HandCurrent.Thumb=HandSetPoint.Thumb+10;
#endif
    }
    //check if connected client send setpoint via BLE
    if (deviceConnected) {
      String Fingers_Value;
      //Set Setpoint  Characteristics value and notify connected client
      //set Pinky value
      log_e("-------------");
      if (ControlMode == speed)
        Fingers_Value = "{\"SP\":\"" + String(HandSetPoint.Pinky) + "\"}";
      else
        Fingers_Value = "{\"PP\":\"" + String(HandSetPoint.Pinky) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e("%s", Fingers_Value);
      //set Ring value
      if (ControlMode == speed)
        Fingers_Value = "{\"SR\":\"" + String(HandSetPoint.Ring) + "\"}";
      else
        Fingers_Value = "{\"PR\":\"" + String(HandSetPoint.Ring) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e("%s", Fingers_Value);
      //set Middle value
      if (ControlMode == speed)
        Fingers_Value = "{\"SM\":\"" + String(HandSetPoint.Middle) + "\"}";
      else
        Fingers_Value = "{\"PM\":\"" + String(HandSetPoint.Middle) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e("%s", Fingers_Value);
      //set Index value
      if (ControlMode == speed)
        Fingers_Value = "{\"SI\":\"" + String(HandSetPoint.Index) + "\"}";
      else
        Fingers_Value = "{\"PI\":\"" + String(HandSetPoint.Index) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e("%s", Fingers_Value);
      //set Thumb value
      if (ControlMode == speed)
        Fingers_Value = "{\"ST\":\"" + String(HandSetPoint.Thumb) + "\"}";
      else
        Fingers_Value = "{\"PT\":\"" + String(HandSetPoint.Thumb) + "\"}";
      Setpoint_Characteristics.setValue(Fingers_Value.c_str());
      Setpoint_Characteristics.notify();
      log_e("%s", Fingers_Value);
      log_e("-------------");
    }
  }
}