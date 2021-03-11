#include <M5StickC.h>　
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secret.h"

#define API_URL "https://zipcloud.ibsnet.co.jp/api/search?zipcode=2120016"

WiFiMulti WiFiMulti;
HTTPClient http;

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 0, 2);

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  M5.Lcd.print("Connecting");

  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
  }

  M5.Lcd.println("");
  M5.Lcd.println(WiFi.localIP());
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    http.begin(API_URL);
    int httpCode = http.GET(); 

    if (httpCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument json_response(1024);
        deserializeJson(json_response, payload);

        JsonObject obj = json_response.as<JsonObject>();
        String status = obj[String("status")];

        M5.Lcd.print("status = "); 
        M5.Lcd.println(status);
    } else {
      M5.Lcd.println("error");
      return;
    }
    
    http.end();
    delay(5000);
  }
}