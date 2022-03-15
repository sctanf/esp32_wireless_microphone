#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "Application.h"
#include "config.h"

#define WIFI_CHECK_INTERVAL_MS 30000

unsigned long lastCheckTime = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting up");
  delay(1000);
  WiFi.hostname(MDNS_DOMAIN);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // disable WiFi sleep mode
  WiFi.setSleep(WIFI_PS_NONE);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

  // startup MDNS
  if (!MDNS.begin(MDNS_DOMAIN))
  {
    Serial.println("MDNS.begin failed");
  }
  Serial.println("Creating microphone");
  Application *application = new Application();
  application->begin();
}



void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi reconnected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

}


void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));

  unsigned long t = millis();
  if (t - WIFI_CHECK_INTERVAL_MS >= lastCheckTime) {
    lastCheckTime = t;
    if (WiFi.status() != WL_CONNECTED) {
      reconnect();
    }
  }

}

