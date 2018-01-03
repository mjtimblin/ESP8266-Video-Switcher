#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Wire.h>

const char* AP_SSID = "ESP8266 Video Switcher";
const char* AP_PASSWORD = "password";
const int MAX_GPIO_PINS = 8;
const int SDA_PIN = 0;
const int SCL_PIN = 2;
const int IO_EXPANDER_ADDRESS = 0x20;

ESP8266WebServer server(80);
int videoSource = 0;

void setup() 
{
  delay(1000);
  Wire.begin(SDA_PIN, SCL_PIN);
  setupServer();
  setupVideoSources();
}

void setupServer()
{
  WiFi.disconnect();
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress myIP = WiFi.softAPIP();
  server.on("/", handleIndex);
  server.on("/video", handleVideo);
  server.onNotFound(handleNotFound);
  server.begin();
}

void setupVideoSources()
{
  // Sets all I/O Expander GPIO pins as outputs
  Wire.beginTransmission(IO_EXPANDER_ADDRESS);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
  changeVideo(videoSource);
}

void handleIndex()
{
  server.send(200, "text/plain", "This is a private connection for internal use only. Please disconnect your device.");
}

void handleVideo()
{
  String command = server.arg("cmd");
  if (command == "next")
  {
    nextVideoSource();
    server.send(200, "text/plain", "Video source changed to " + (String)(videoSource + 1));
  }
  else
  {
    server.send(422, "text/plain", "422: Missing or invalid parameter");
  }
}

void handleNotFound()
{
  server.send(404, "text/plain", "404: Not found");
}

void nextVideoSource()
{
  videoSource = (videoSource + 1) % MAX_GPIO_PINS;
  changeVideo(videoSource);
}

void changeVideo(int source)
{
  Wire.beginTransmission(IO_EXPANDER_ADDRESS);
  Wire.write(0x09);
  switch(source)
  {
    case 0:
      Wire.write(0x01);
      break;
    case 1:
      Wire.write(0x02);
      break;
    case 2:
      Wire.write(0x04);
      break;
    case 3:
      Wire.write(0x08);
      break;
    case 4:
      Wire.write(0x10);
      break;
    case 5:
      Wire.write(0x20);
      break;
    case 6:
      Wire.write(0x40);
      break;
    case 7:
      Wire.write(0x80);
      break;
  }
  Wire.endTransmission();
}

void loop() 
{
  server.handleClient();
}
