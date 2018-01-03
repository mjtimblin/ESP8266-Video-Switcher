#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Bounce2.h>

const int BUTTON_PIN = 2; // Button input on ESP8266 GPIO2
const char* AP_SSID = "ESP8266 Video Switcher";
const char* AP_PASSWORD = "password";
const String HOST = "192.168.4.1";
const int PORT = 80;

Bounce debouncer = Bounce();
bool previousButtonState = true;

void setup() 
{
  Serial.begin(9600);
  Serial.println();
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASSWORD);  
  Serial.print("Connecting..");
 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n Connected");
}

void sendNextCommand()
{
  HTTPClient http; 
  http.begin(HOST, PORT, "/video");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  int httpCode = http.POST("cmd=next");
  if (httpCode > 0)
  {
    String payload = http.getString();  
    Serial.println(payload); 
  }
  else 
  {
    Serial.println("Request failed, error: " + http.errorToString(httpCode));
  }
  http.end();
}

void loop() 
{
  debouncer.update();
  bool currentButtonState = debouncer.read();
  if (previousButtonState && !currentButtonState)
  {
    Serial.println("Button pressed");
    sendNextCommand();
  }
  previousButtonState = currentButtonState;
}

