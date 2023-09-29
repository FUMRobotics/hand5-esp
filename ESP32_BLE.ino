/*********
writen by MH.Taji
*********/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <Arduino_JSON.h>

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

// Pinky Characteristic and Descriptor
  BLECharacteristic Finger_Pinky_Characteristics("e9eb3de6-5eda-11ee-8c99-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor Finger_Pinky_Descriptor(BLEUUID((uint16_t)0x2901));

// Ring Characteristic and Descriptor
  BLECharacteristic Finger_Ring_Characteristics("e9eb3f4e-5eda-11ee-8c99-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor Finger_Ring_Descriptor(BLEUUID((uint16_t)0x2902));

// Middle Characteristic and Descriptor
  BLECharacteristic Finger_Middle_Characteristics("e9eb46b0-5eda-11ee-8c99-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor Finger_Middle_Descriptor(BLEUUID((uint16_t)0x2903));

// Index Characteristic and Descriptor
  BLECharacteristic Finger_Index_Characteristics("e9eb482c-5eda-11ee-8c99-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor Finger_Index_Descriptor(BLEUUID((uint16_t)0x2904));

// Thumb Characteristic and Descriptor
  BLECharacteristic Finger_Thumb_Characteristics("e9eb499e-5eda-11ee-8c99-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor Finger_Thumb_Descriptor(BLEUUID((uint16_t)0x2905));


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

  // Create BLE Characteristics and Create a BLE Descriptor
  //Pinky
    HANDService->addCharacteristic(&Finger_Pinky_Characteristics);
    Finger_Pinky_Descriptor.setValue("Pinky Finger");
    Finger_Pinky_Characteristics.addDescriptor(&Finger_Pinky_Descriptor);
  //Ring
    HANDService->addCharacteristic(&Finger_Ring_Characteristics);
    Finger_Ring_Descriptor.setValue("Ring Finger");
    Finger_Ring_Characteristics.addDescriptor(&Finger_Ring_Descriptor);
  //Middle
    HANDService->addCharacteristic(&Finger_Middle_Characteristics);
    Finger_Middle_Descriptor.setValue("Middle Finger");
    Finger_Middle_Characteristics.addDescriptor(&Finger_Middle_Descriptor);
  //Index
    HANDService->addCharacteristic(&Finger_Index_Characteristics);
    Finger_Index_Descriptor.setValue("Index Finger");
    Finger_Index_Characteristics.addDescriptor(&Finger_Index_Descriptor);
  //Thumb
    HANDService->addCharacteristic(&Finger_Thumb_Characteristics);
    Finger_Thumb_Descriptor.setValue("Thumb Finger");
    Finger_Thumb_Characteristics.addDescriptor(&Finger_Thumb_Descriptor);

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
      
      static char Fingers_Value[6];

        //Set Pinky Characteristic value and notify connected client
        dtostrf(HandFinger.Pinky, 6, 2, Fingers_Value);
        Finger_Pinky_Characteristics.setValue(Fingers_Value);
        Finger_Pinky_Characteristics.notify();
        Serial.print(" Pinky :");
        Serial.print(HandFinger.Pinky);
        //Set Ring Characteristic value and notify connected client
        dtostrf(HandFinger.Ring, 6, 2, Fingers_Value);
        Finger_Ring_Characteristics.setValue(Fingers_Value);
        Finger_Ring_Characteristics.notify();
        Serial.print(" Ring :");
        Serial.print(HandFinger.Ring);
        //Set Middle Characteristic value and notify connected client
        dtostrf(HandFinger.Middle, 6, 2, Fingers_Value);
        Finger_Middle_Characteristics.setValue(Fingers_Value);
        Finger_Middle_Characteristics.notify();
        Serial.print(" Middle :");
        Serial.print(HandFinger.Middle);
        //Set Pinky Characteristic value and notify connected client
        dtostrf(HandFinger.Index, 6, 2, Fingers_Value);
        Finger_Index_Characteristics.setValue(Fingers_Value);
        Finger_Index_Characteristics.notify();
        Serial.print(" Index :");
        Serial.print(HandFinger.Index);
        //Set Thumb Characteristic value and notify connected client
        dtostrf(HandFinger.Thumb, 6, 2, Fingers_Value);
        Finger_Thumb_Characteristics.setValue(Fingers_Value);
        Finger_Thumb_Characteristics.notify();
        Serial.print(" Thumb :");
        Serial.println(HandFinger.Thumb);

      lastTime = millis();
    }
  }
}