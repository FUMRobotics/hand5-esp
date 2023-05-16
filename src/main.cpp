/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>

// Replace with your network credentials
const char *ssid = "HAND";
const char *password = "fum";
/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
// Set LED GPIO
bool SendFingerStates = 0;
// Stores fingers state
DynamicJsonDocument FingerStateJson(10000);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Updates DHT readings every 10 seconds
const long interval = 10000;
AsyncWebParameter *response;
/*********************|Finger struct|**********************/


struct FingerStateStruct
{
  String Thumb;
  String Middele;
  String Index;
  String Ring;
  String Pinky;
};
struct FingerStateStruct structFingerState;

/*******************|END Finger struct|*******************/
//-------------------|html page|---------------------------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>ESP32 HTML Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>

<body>
  <form action="/Thumb">
    Thumb Finger's position: <input type="number" name="ThumbValue"><br>
  </form><br>
  <form action="/Middele">
    Middle Finger's position: <input type="number" name="MiddleValue"><br>
  </form><br>
  <form action="/Index">
    Index Finger's position: <input type="number" name="IndexValue"><br>
  </form><br>
  <form action="/Ring">
    Ring Finger's position: <input type="number" name="RingValue"><br>
  </form><br>
  <form action="/Pinky">
    Pinky Finger's position: <input type="number" name="PinkyValue"><br>
</form><br>
<input type="submit" value="STOP" name="ReadStatus">
</body>

</html>
)rawliteral";
//-------------------|END html page|------------------------

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);

  //  WiFi.disconnect(); // forget the persistent connection to test the Configuration AP

  // Configures static IP address
  if (!WiFi.softAPConfig(local_ip, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }
  // Initialize SPIFFS
  WiFi.softAP("Hand", "FUM");
  Serial.println();
  Serial.print("Server IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Server MAC address: ");
  Serial.println(WiFi.softAPmacAddress());
  // first value***********************************************
  structFingerState.Index = "0";
  structFingerState.Middele = "0";
  structFingerState.Pinky = "0";
  structFingerState.Ring = "0";
  structFingerState.Thumb = "0";
  //***********************************************************
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  //-----------------|Thumb Finger|--------------------------
  server.on("/Thumb", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("ThumbValue");
    structFingerState.Thumb =response->value();
      SendFingerStates=1;

  //-----------------|Refresh Page|----------------------------
    request->send_P(200, "text/html", index_html); });

  //-----------------|Middele Finger|--------------------------
  server.on("/Middele", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("MiddleValue");
    structFingerState.Middele=response->value();
      SendFingerStates=1;
  //-----------------|Refresh Page|---------------------------
    request->send_P(200, "text/html", index_html); });
  //-----------------|Index Finger|---------------------------
  server.on("/Index", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("IndexValue");
    structFingerState.Index=response->value();
      SendFingerStates=1;
  //-----------------|Refresh Page|---------------------------
  SendFingerStates=1;
    request->send_P(200, "text/html", index_html); });
  //-----------------|Ring Finger|----------------------------
  server.on("/Ring", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("RingValue");
    structFingerState.Ring=response->value();
      SendFingerStates=1;
  //-----------------|Refresh Page|----------------------------
    request->send_P(200, "text/html", index_html); });
  //-----------------|Pinky Finger|----------------------------
  server.on("/Pinky", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("PinkyValue");
    structFingerState.Pinky=response->value();
      SendFingerStates=1;
  //-----------------|Refresh Page|----------------------------
    request->send_P(200, "text/html", index_html); });

  // Start server
  server.begin();

  WiFi.waitForConnectResult(3000);
  Serial.println("HTTP server started");
}

void loop()
{
  if (SendFingerStates)
  {
    // String json_string;
    //-----------------|Thumb Finger|--------------------------
    if(structFingerState.Thumb==0)
    {
      structFingerState.Thumb="0";
    }else if(structFingerState.Thumb>"100")
    {
      structFingerState.Thumb="100";
    }
    FingerStateJson["Thumb"]["Value"] = structFingerState.Thumb;
    // Serial.println(structFingerState.Thumb.Status);
    // Serial.println(structFingerState.Thumb.Value);
    //-----------------|Middele Finger|--------------------------
    if(structFingerState.Middele==0)
    {
      structFingerState.Middele="0";
    }else if(structFingerState.Middele>"100")
    {
      structFingerState.Middele="100";
    }
    FingerStateJson["Middele"]["Value"] = structFingerState.Middele;
    // Serial.println(structFingerState.Middele.Status);
    // Serial.println(structFingerState.Middele.Value);
    //-----------------|Index Finger|---------------------------
    if(structFingerState.Index==0)
    {
      structFingerState.Index="0";
    }else if(structFingerState.Index>"100")
    {
      structFingerState.Index="100";
    }
    FingerStateJson["Index"]["Value"] = structFingerState.Index;
    // Serial.println(structFingerState.Index.Status);
    // Serial.println(structFingerState.Index.Value);
    //-----------------|Ring Finger|----------------------------
    if(structFingerState.Ring==0)
    {
      structFingerState.Ring="0";
    }else if(structFingerState.Ring>"100")
    {
      structFingerState.Ring="100";
    }
    FingerStateJson["Ring"]["Value"] = structFingerState.Ring;
    // Serial.println(structFingerState.Ring.Status);
    // Serial.println(structFingerState.Ring.Value);
    //-----------------|Pinky Finger|----------------------------
    if(structFingerState.Pinky==0)
    {
      structFingerState.Pinky="0";
    }else if(structFingerState.Pinky>"100")
    {
      structFingerState.Pinky="100";
    }
    FingerStateJson["Pinky"]["Value"] = structFingerState.Pinky;
    // Serial.println(structFingerState.Pinky.Status);
    // Serial.println(structFingerState.Pinky.Value);
    char json_string[256];
    serializeJson(FingerStateJson, json_string);
    Serial.println(json_string);
    SendFingerStates = 0;
  }
}