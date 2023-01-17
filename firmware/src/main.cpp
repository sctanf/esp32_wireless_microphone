#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "Application.h"
#include "config.h"
#include <EEPROM.h>
#include "flashdata.h"

#define WIFI_CHECK_INTERVAL_MS 60000



//
// EEPROM data
//
FlashData data;


unsigned long lastCheckTime = 0;




void readDataFromEeprom() {
  // read data from EEPROM
  // commit 512 bytes of ESP8266 flash (for "EEPROM" emulation)
  // this step actually loads the content (512 bytes) of flash into
  // a 512-byte-array cache in RAM
  EEPROM.begin(EPROM_SIZE);
  EEPROM.get(0, data);
  // check for already saved data, if not exists, use default values
  if (data.saved != CFG_HOLD) {
    // default values
    data.saved = CFG_HOLD; // arbitrary number to be different than zero
    strcpy(data.ssid, WIFI_SSID);
    strcpy(data.password, WIFI_PASSWORD);
    EEPROM.put(0, data);
  }
  EEPROM.commit();  
  Serial.printf("EEPROM CFG:%d -> %s : %s \r\n", data.saved, data.ssid, data.password);
}



void connectWifi() {

  readDataFromEeprom();

  Serial.println("Setup WiFi");
  Serial.print("Connecting to ");

  //TODO read values from EPROM
  Serial.println(data.ssid);
  WiFi.begin(data.ssid, data.password);

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

  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting up");
  delay(1000);

  // disable WiFi sleep mode
  WiFi.setSleep(WIFI_PS_NONE);
  // it is extremely important to set the hostname right before calling the mode
  // otherwise, it will not work and the DHCP request will contain 'esp32-aabbcc'
  // you can verify this by running 
  //    tcpdump -i eth0 udp port 67 -vvv
  // on the router just before the esp boot
  // another issue will be visible: getting 502 (bad gateway) from the nginx when forwarding the request
  WiFi.setHostname(MDNS_DOMAIN);
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);  
  
  connectWifi();
  
  delay(500);

  // startup MDNS
  if (!MDNS.begin(MDNS_DOMAIN))
  {
    Serial.println("ERROR: MDNS.begin failed");
  }
  delay(10);

  Serial.println("Creating microphone");
  Application *application = new Application();
  application->begin();

  Serial.print("Connect to TCP socket http://");
  Serial.print(MDNS_DOMAIN);
  Serial.println(".local:9090/ to try out TCP socket streaming");

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

