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
const char *password = "FUM_HAND";
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
  <title>FUM HAND</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>

<body>
  
    <form action="/Send">
    Thumb Finger's position: <input type="number" name="ThumbValue"><br>
<br>
    Middle Finger's position: <input type="number" name="MiddleValue"><br>
<br>
    Index Finger's position: <input type="number" name="IndexValue"><br>
<br>
    Ring Finger's position: <input type="number" name="RingValue"><br>
<br>
    Pinky Finger's position: <input type="number" name="PinkyValue"><br>
<br>
    <input type="submit" value="SEND">
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
  WiFi.softAP(ssid,password);
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

  //-----------------|Read fingers value|-----------------------
  server.on("/Send", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  response = request->getParam("PinkyValue");
    structFingerState.Pinky=response->value();
      response = request->getParam("RingValue");
    structFingerState.Ring=response->value();
      response = request->getParam("IndexValue");
    structFingerState.Index=response->value();
      response = request->getParam("MiddleValue");
    structFingerState.Middele=response->value();
      response = request->getParam("ThumbValue");
    structFingerState.Thumb =response->value();
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
    uint FingerValue=0;
    // String json_string;
    //-----------------|Thumb Finger|--------------------------
    if(structFingerState.Thumb==0)
    {
      structFingerState.Thumb="0";
    }else if(structFingerState.Thumb.toInt()>100)
    {
      structFingerState.Thumb="100";
    }
    FingerValue=structFingerState.Thumb.toInt();
    FingerStateJson["Thumb"]["Value"] =String(FingerValue);
    //-----------------|Middele Finger|--------------------------
    if(structFingerState.Middele==0)
    {
      structFingerState.Middele="0";
    }else if(structFingerState.Middele.toInt()>100)
    {
      structFingerState.Middele="100";
    }
    FingerValue=structFingerState.Middele.toInt();
    FingerStateJson["Middele"]["Value"]= String(FingerValue);
    //-----------------|Index Finger|---------------------------
    if(structFingerState.Index==0)
    {
      structFingerState.Index="0";
    }else if(structFingerState.Index.toInt()>100)
    {
      structFingerState.Index="100";
    }
    FingerValue=structFingerState.Index.toInt();
    FingerStateJson["Index"]["Value"] = String(FingerValue);
    //-----------------|Ring Finger|----------------------------
    if(structFingerState.Ring==0)
    {
      structFingerState.Ring="0";
    }else if(structFingerState.Ring.toInt()>100)
    {
      structFingerState.Ring="100";
    }
    FingerValue=structFingerState.Ring.toInt();
    FingerStateJson["Ring"]["Value"] = String(FingerValue);
    //-----------------|Pinky Finger|----------------------------
    if(structFingerState.Pinky==0)
    {
      structFingerState.Pinky="0";
    }else if(structFingerState.Pinky.toInt()>100)
    {
      structFingerState.Pinky="100";
    }
    FingerValue=structFingerState.Pinky.toInt();
    FingerStateJson["Pinky"]["Value"] = String(FingerValue);
    
    /*creat JSON struct */
    char json_string[256];
    serializeJson(FingerStateJson, json_string);
    /*send JSON struct */
    Serial.println(json_string);
    SendFingerStates = 0;
  }
}