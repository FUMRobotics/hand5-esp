/*********
writen by MH.Taji
BLE Server
*********/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
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

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

bool deviceConnected = false;

// See the following for generating UUIDs:
#define SERVICE_UUID "e9eb3b02-5eda-11ee-8c99-0242ac120002"


// Position Characteristic and Descriptor
  BLECharacteristic Position_Characteristics("e9eb3f4e-5eda-11ee-8c99-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor Position_Descriptor(BLEUUID((uint16_t)0x2903));

// Current Characteristic and Descriptor
  BLECharacteristic Current_Characteristics("e9eb46b0-5eda-11ee-8c99-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor Current_Descriptor(BLEUUID((uint16_t)0x2904));

// setpoint Characteristic and Descriptor
  BLECharacteristic Setpoint_Characteristics("e9eb482c-5eda-11ee-8c99-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor Setpoint_Descriptor(BLEUUID((uint16_t)0x2905));


//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  // Start serial communication 
  Serial.begin(115200);

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
  //Current
    HANDService->addCharacteristic(&Current_Characteristics);
    Current_Descriptor.setValue("Current");
    Current_Characteristics.addDescriptor(&Current_Descriptor);
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
  Serial.println("Waiting a client connection to notify...");

}

void loop() {
  if (deviceConnected) {
    if ((millis() - lastTime) > timerDelay) {
      
      HandFinger.Pinky++;
      HandFinger.Ring++;
      HandFinger.Middle++;
      HandFinger.Index++;
      HandFinger.Thumb++;
      
      String Fingers_Value;

        //Set Setpoint  Characteristic value and notify connected client
        //set Pinky value 
        Fingers_Value="{\"P\":\""+String(HandFinger.Pinky*100)+"\"}";
        Setpoint_Characteristics.setValue(Fingers_Value.c_str());
        Setpoint_Characteristics.notify();
        Serial.print(Fingers_Value);
        //set Ring value 
        Fingers_Value="{\"R\":\""+String(HandFinger.Ring*100)+"\"}";
        Setpoint_Characteristics.setValue(Fingers_Value.c_str());
        Setpoint_Characteristics.notify();
        Serial.print(Fingers_Value);
        //set Middle value 
        Fingers_Value="{\"M\":\""+String(HandFinger.Middle*100)+"\"}";
        Setpoint_Characteristics.setValue(Fingers_Value.c_str());
        Setpoint_Characteristics.notify();
        Serial.print(Fingers_Value);
        //set Index value 
        Fingers_Value="{\"I\":\""+String(HandFinger.Index*100)+"\"}";
        Setpoint_Characteristics.setValue(Fingers_Value.c_str());
        Setpoint_Characteristics.notify();
        Serial.print(Fingers_Value);
        //set Thumb value 
        Fingers_Value="{\"T\":\""+String(HandFinger.Thumb*100)+"\"}";
        Setpoint_Characteristics.setValue(Fingers_Value.c_str());
        Setpoint_Characteristics.notify();
        Serial.print(Fingers_Value);

      lastTime = millis();
    }
  }
}