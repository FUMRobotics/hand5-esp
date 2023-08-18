#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "HAND";
const char *password = "FUM_HAND";

// Your Domain name with URL path or IP address with path
String serverName = "http://192.168.1.1/Send";

String ptr;
StaticJsonDocument<200> doc;
DeserializationError error;
struct
{
  uint8_t Pinky;
  uint8_t Ring;
  uint8_t Middle;
  uint8_t Index;
  uint8_t Thumb;
} HandFinger;
void setup()
{
  Serial2.begin(115200);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (Serial2.available())
  {
    ptr = Serial2.readStringUntil('\n');
    Serial.print(ptr);
    error = deserializeJson(doc, ptr);
    HandFinger.Index = doc["Index"];
    HandFinger.Middle = doc["Middle"];
    HandFinger.Pinky = doc["Pinky"];
    HandFinger.Ring = doc["Ring"];
    HandFinger.Thumb = doc["Thumb"];
    if (error)
    {
      Serial.print(F("\ndeserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    //---------------------------- WIFI Communnication ---------------------------------------------
    if (doc["Communication"] == "WIFI")
    {
      Serial.println("\n\nwifi start");
      // Check WiFi connection status
      if (WiFi.status() == WL_CONNECTED)
      {
        HTTPClient http;

        String serverPath = serverName + "?ThumbValue=" + HandFinger.Thumb + "&MiddleValue=" + HandFinger.Middle + "&IndexValue=" + HandFinger.Index + "&RingValue=" + HandFinger.Ring + "&PinkyValue=" + HandFinger.Pinky;

        // Your Domain name with URL path or IP address with path
        http.begin(serverPath.c_str());

        // If you need Node-RED/server authentication, insert user and password below
        // http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

        // Send HTTP GET request
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        }
        else
        {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();
      }
      else
      {
        Serial.println("WiFi Disconnected");
      }
    }
    //---------------------------- Bluetooth Communnication ---------------------------------------------
    if (doc["Communication"] == "Bluetooth")
    {
      Serial.println("\n\nbluetooth start");
    }
  }
}