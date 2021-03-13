#include <M5StickC.h>　
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secret.h"

#define API_URL "https://api.switch-bot.com/v1.0/devices/FA98475977EC/commands"

WiFiMulti WiFiMulti;
HTTPClient http;

int PIN = 33;
int PRESS_SB_THRESHOLD = 3500;
bool pressed_sb = false;

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 0, 2);

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  M5.Lcd.print("Connecting");

  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
  }

  // PIN setting
  pinMode(PIN, ANALOG);

  // clear LCD
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);

  // show IP address
  M5.Lcd.print("IP addr: ");
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.println("---");

  // show initial api message
  M5.Lcd.println("status: -"); 
  M5.Lcd.println("message: -"); 
}

void loop() {
  int luminance = analogRead(PIN);

  if (!pressed_sb && luminance < PRESS_SB_THRESHOLD && WiFi.status() == WL_CONNECTED) {

    // create request
    const int capacity = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<capacity> json_request;
    json_request["command"] = "press";
    json_request["parameter"] = "default";
    json_request["commandType"] = "command";
    char buffer[255];
    serializeJson(json_request, buffer, sizeof(buffer));

    http.begin(API_URL);
    http.addHeader("Authorization", API_TOKEN);
    http.addHeader("Content-Type", "application/json; charset=utf8");
    
    int httpCode = http.POST((uint8_t*)buffer, strlen(buffer));

    if (httpCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument json_response(1024);
        deserializeJson(json_response, payload);

        JsonObject obj = json_response.as<JsonObject>();
        String status = obj[String("statusCode")];
        String message = obj[String("message")];

        M5.Lcd.setCursor(0, 32);
        M5.Lcd.print("status: "); 
        M5.Lcd.println(status);
        M5.Lcd.print("message: "); 
        M5.Lcd.println(message);

        pressed_sb = true;
    } else {
      M5.Lcd.println("error");
      return;
    }
    
    http.end();
    delay(5000);
  } else if (luminance >= PRESS_SB_THRESHOLD) {
    pressed_sb = false;
  }

  // show luminamce
  M5.Lcd.setCursor(0, 65);
  M5.Lcd.print("luminance: "); 
  M5.Lcd.println(luminance);
}