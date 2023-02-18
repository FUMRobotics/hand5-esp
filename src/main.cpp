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
// Replaces placeholder with LED state value
/*********************|Finger struct|**********************/
struct StatusFinger
{
  String Value;
  String Status;
};

struct FingerStateStruct
{
  StatusFinger Thumb;
  StatusFinger Middele;
  StatusFinger Index;
  StatusFinger Ring;
  StatusFinger Pinky;
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
    Thumb Finger: <input type="number" name="ThumbValue"><br>
    <input type="submit" value="OPEN" name="ThumbStatus">
    <input type="submit" value="CLOSE" name="ThumbStatus">
    <input type="submit" value="STOP" name="ThumbStatus">
  </form><br>
  <form action="/Middele">
    Middle Finger: <input type="number" name="MiddleValue"><br>
    <input type="submit" value="OPEN" name="MiddleStatus">
    <input type="submit" value="CLOSE" name="MiddleStatus">
    <input type="submit" value="STOP" name="MiddleStatus">
  </form><br>
  <form action="/Index">
    Index Finger: <input type="number" name="IndexValue"><br>
    <input type="submit" value="OPEN" name="IndexStatus">
    <input type="submit" value="CLOSE" name="IndexStatus">
    <input type="submit" value="STOP" name="IndexStatus">
  </form><br>
  <form action="/Ring">
    Ring Finger: <input type="number" name="RingValue"><br>
    <input type="submit" value="OPEN" name="RingStatus">
    <input type="submit" value="CLOSE" name="RingStatus">
    <input type="submit" value="STOP" name="RingStatus">
  </form><br>
  <form action="/Pinky">
    Pinky Finger: <input type="number" name="PinkyValue"><br>
    <input type="submit" value="OPEN" name="PinkyStatus">
    <input type="submit" value="CLOSE" name="PinkyStatus">
    <input type="submit" value="STOP" name="PinkyStatus">
  </form><br>
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
  structFingerState.Index.Status = "STOP";
  structFingerState.Middele.Status = "STOP";
  structFingerState.Pinky.Status = "STOP";
  structFingerState.Ring.Status = "STOP";
  structFingerState.Thumb.Status = "STOP";
  //***********************************************************
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  //-----------------|Thumb Finger|--------------------------
  server.on("/Thumb", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("ThumbValue");
    structFingerState.Thumb.Value =response->value();
  response = request->getParam("ThumbStatus");
    structFingerState.Thumb.Status=response->value();
      SendFingerStates=1;

  //-----------------|Refresh Page|----------------------------
    request->send_P(200, "text/html", index_html); });

  //-----------------|Middele Finger|--------------------------
  server.on("/Middele", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("MiddleValue");
    structFingerState.Middele.Value=response->value();
  response = request->getParam("MiddleStatus");
    structFingerState.Middele.Status=response->value();
      SendFingerStates=1;
  //-----------------|Refresh Page|---------------------------
    request->send_P(200, "text/html", index_html); });
  //-----------------|Index Finger|---------------------------
  server.on("/Index", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("IndexValue");
    structFingerState.Index.Value=response->value();
  response = request->getParam("IndexStatus");
    structFingerState.Index.Status=response->value();
      SendFingerStates=1;
  //-----------------|Refresh Page|---------------------------
  SendFingerStates=1;
    request->send_P(200, "text/html", index_html); });
  //-----------------|Ring Finger|----------------------------
  server.on("/Ring", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("RingValue");
    structFingerState.Ring.Value=response->value();
  response = request->getParam("RingStatus");
    structFingerState.Ring.Status=response->value();
      SendFingerStates=1;
  //-----------------|Refresh Page|----------------------------
    request->send_P(200, "text/html", index_html); });
  //-----------------|Pinky Finger|----------------------------
  server.on("/Pinky", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("PinkyValue");
    structFingerState.Pinky.Value=response->value();
  response = request->getParam("PinkyStatus");
    structFingerState.Pinky.Status=response->value();
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
    if(structFingerState.Thumb.Status=="null")
    {
      structFingerState.Thumb.Status="STOP";      
    }
    if(structFingerState.Thumb.Value==0)
    {
      structFingerState.Thumb.Value="0";
    }
    FingerStateJson["Thumb"]["Status"] = structFingerState.Thumb.Status;
    FingerStateJson["Thumb"]["Value"] = structFingerState.Thumb.Value;
    // Serial.println(structFingerState.Thumb.Status);
    // Serial.println(structFingerState.Thumb.Value);
    //-----------------|Middele Finger|--------------------------
    if(structFingerState.Middele.Status=="null")
    {
      structFingerState.Middele.Status="STOP";      
    }
    if(structFingerState.Middele.Value==0)
    {
      structFingerState.Middele.Value="0";
    }
    FingerStateJson["Middele"]["Status"] = structFingerState.Middele.Status;
    FingerStateJson["Middele"]["Value"] = structFingerState.Middele.Value;
    // Serial.println(structFingerState.Middele.Status);
    // Serial.println(structFingerState.Middele.Value);
    //-----------------|Index Finger|---------------------------
    if(structFingerState.Index.Status=="null")
    {
      structFingerState.Index.Status="STOP";      
    }
    if(structFingerState.Index.Value==0)
    {
      structFingerState.Index.Value="0";
    }
    FingerStateJson["Index"]["Status"] = structFingerState.Index.Status;
    FingerStateJson["Index"]["Value"] = structFingerState.Index.Value;
    // Serial.println(structFingerState.Index.Status);
    // Serial.println(structFingerState.Index.Value);
    //-----------------|Ring Finger|----------------------------
    if(structFingerState.Ring.Status=="null")
    {
      structFingerState.Ring.Status="STOP";      
    }
    if(structFingerState.Ring.Value==0)
    {
      structFingerState.Ring.Value="0";
    }
    FingerStateJson["Ring"]["Status"] = structFingerState.Ring.Status;
    FingerStateJson["Ring"]["Value"] = structFingerState.Ring.Value;
    // Serial.println(structFingerState.Ring.Status);
    // Serial.println(structFingerState.Ring.Value);
    //-----------------|Pinky Finger|----------------------------
    if(structFingerState.Pinky.Status=="null")
    {
      structFingerState.Pinky.Status="STOP";      
    }
    if(structFingerState.Pinky.Value==0)
    {
      structFingerState.Pinky.Value="0";
    }
    FingerStateJson["Pinky"]["Status"] = structFingerState.Pinky.Status;
    FingerStateJson["Pinky"]["Value"] = structFingerState.Pinky.Value;
    // Serial.println(structFingerState.Pinky.Status);
    // Serial.println(structFingerState.Pinky.Value);
    char json_string[256];
    serializeJson(FingerStateJson, json_string);
    Serial.println(json_string);
    SendFingerStates = 0;
  }
}