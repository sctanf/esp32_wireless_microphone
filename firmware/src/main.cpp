#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "Application.h"
#include "config.h"

#define WIFI_CHECK_INTERVAL_MS 30000

unsigned long lastCheckTime = 0;



void connectWifi() {
  Serial.println("Setup WiFi");
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int waitMs = 60000;
  while (WiFi.status() != WL_CONNECTED && waitMs > 0) {
    delay(500);
    waitMs -= 500;
    Serial.print(".");
  }
  if (waitMs <= 0) {
    Serial.println("Restart.");
    ESP.restart();
  }
  Serial.println();
  Serial.print("WiFi connected, IP: ");
  Serial.println(WiFi.localIP());
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting up");
  delay(1000);

  // disable WiFi sleep mode
  WiFi.setSleep(WIFI_PS_NONE);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(MDNS_DOMAIN);
  connectWifi();
  
  delay(500);

  // startup MDNS
  if (!MDNS.begin(MDNS_DOMAIN))
  {
    Serial.println("MDNS.begin failed");
  }
  delay(10);

  Serial.println("Creating microphone");
  Application *application = new Application();
  application->begin();
}


void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));

  unsigned long t = millis();
  if (t - WIFI_CHECK_INTERVAL_MS >= lastCheckTime) {
    lastCheckTime = t;
    if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
    }
  }

}

