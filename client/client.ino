/* 
 * This is code for the client (remote) for a video switcher based on ESP8266 models for communication.
 * The entire project can be found at https://github.com/mjtimblin/ESP8266-Video-Switcher. I had some 
 * inspiration from https://www.geekstips.com/two-esp8266-communication-talk-each-other/. Check out 
 * their project for more information. 
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Bounce2.h>

const char* AP_SSID = "video-switcher";
const char* AP_PASSWORD = "esp8266";
const String SERVER_URL = "http://192.168.4.1/video";
const int BUTTON_PIN = 2; // Button input on ESP8266 GPIO2

Bounce debouncer = Bounce();
IPAddress ip(192, 168, 4, 4);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() 
{
  bool setupSuccess;
  ESP.eraseConfig();
  WiFi.persistent(false);
  Serial.begin(74880);
  setupButton();
  setupSuccess = setupWifi();
  if (!setupSuccess)
  {
    while(1);
  }
}

void setupButton()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);
}

bool setupWifi()
{
  bool result = true;
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(AP_SSID, AP_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    if(attempts > 20)
    {
      result = false;
      break;
    }
    delay(500);
    attempts++;
  }
  return result;
}

void sendNextCommand()
{
 HTTPClient http;
 http.begin(SERVER_URL);
 http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 http.POST("cmd=next"); 
 http.writeToStream(&Serial);
 http.end();
}

void loop() 
{
  debouncer.update();
  if (debouncer.read() == LOW)
  {
    sendNextCommand();
  }
}

