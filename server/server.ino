/* 
 * This is code for the server (video switch) for a video switcher project based on ESP8266 modules for 
 * communication. The entire project can be found at https://github.com/mjtimblin/ESP8266-Video-Switcher. 
 * I had some inspiration from https://www.geekstips.com/two-esp8266-communication-talk-each-other/. 
 * Check out their project for more information. 
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Wire.h>

const char* AP_SSID = "ESP8266 Video Switcher";
const char* AP_PASSWORD = "password";
const int MAX_VIDEO_SOURCES = 8;
const int SDA_PIN = 0;
const int SCL_PIN = 2;

ESP8266WebServer server(80);
int videoSource = 0;

void setup() 
{
  delay(1000);
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(9600);
  setupServer();
  setupVideoSources();
}

void setupServer()
{
  Serial.println("Starting server");
  WiFi.disconnect();
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleIndex);
  server.on("/video", handleVideo);
  server.begin();
}

void setupVideoSources()
{
  Wire.beginTransmission(0x20); //begins talking to the slave device
  Wire.write(0x00); //selects the IODIRA register
  Wire.write(0x00); //this sets the first 3 pins as inputs and the rest as outputs
  Wire.endTransmission(); //stops talking to device
  setVideoSource(videoSource);
}

void handleIndex()
{
  server.send(200, "text/plain", "This is a private connection. Please disconnect your device.");
}

void handleVideo()
{
  String command = server.arg("cmd");
  if (command == "next")
  {
    videoSource = (videoSource + 1) % MAX_VIDEO_SOURCES;
    setVideoSource(videoSource);
    server.send(200, "text/plain", "Video source changed to " + (String)(videoSource + 1));
  }
  else
  {
    server.send(200, "text/plain", "Invalid command");
  }
}

void setVideoSource(int source)
{
  Serial.println("Changing video source to " + (String)source);
  Wire.beginTransmission(0x20);
  Wire.write(0x09); //selects the GPIO pins
  switch(source)
  {
    case 0:
      Wire.write(0b00000001);
      break;
    case 1:
      Wire.write(0b00000010);
      break;
    case 2:
      Wire.write(0b00000100);
      break;
    case 3:
      Wire.write(0b00001000);
      break;
    case 4:
      Wire.write(0b00010000);
      break;
    case 5:
      Wire.write(0b00100000);
      break;
    case 6:
      Wire.write(0b01000000);
      break;
    case 7:
      Wire.write(0b10000000);
      break;
  }
  Wire.endTransmission(); //ends communication with the device
}

void loop() 
{
  server.handleClient();
}
