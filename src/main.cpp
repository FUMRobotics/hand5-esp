#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *WIFIssid = "HAND";
const char *WIFIpassword = "FUM_HAND";

// Your Domain name with URL path or IP address with path
String WIFIserverName = "http://192.168.1.1/Send";

String UartString;
StaticJsonDocument<200> JSONstruct;
DeserializationError JSONerror;
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
  WiFi.begin(WIFIssid, WIFIpassword);
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
    UartString = Serial2.readStringUntil('\n');
    JSONerror = deserializeJson(JSONstruct, UartString);
    if (JSONerror)
    {
      Serial.print(F("\ndeserializeJson() failed: "));
      Serial.println(JSONerror.f_str());
      return;
    }
    HandFinger.Index = JSONstruct["Index"];
    HandFinger.Middle = JSONstruct["Middle"];
    HandFinger.Pinky = JSONstruct["Pinky"];
    HandFinger.Ring = JSONstruct["Ring"];
    HandFinger.Thumb = JSONstruct["Thumb"];
    //---------------------------- WIFI Communnication ---------------------------------------------
    if (JSONstruct["Communication"] == "WIFI")
    {
      Serial.println("\n\nwifi start");
      // Check WiFi connection status
      if (WiFi.status() == WL_CONNECTED)
      {
        HTTPClient http;

        String serverPath = WIFIserverName + "?ThumbValue=" + HandFinger.Thumb + "&MiddleValue=" + HandFinger.Middle + "&IndexValue=" + HandFinger.Index + "&RingValue=" + HandFinger.Ring + "&PinkyValue=" + HandFinger.Pinky;

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
    if (JSONstruct["Communication"] == "Bluetooth")
    {
      Serial.println("\n\nbluetooth start");
    }
  }
}