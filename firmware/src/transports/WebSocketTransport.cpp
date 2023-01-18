#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "WebSocketTransport.h"
#include "flashdata.h"
#include <EEPROM.h>

void WebSocketTransport::begin()
{
  server = new AsyncWebServer(80);
  ws = new AsyncWebSocket("/audio_stream");
  
  // start off spiffs so we can serve the static files
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS.begin failed");
  } else {
    Serial.println("SPIFFS mounted");
  }

  // setup cors to allow connections from anywhere - handy for testing from localhost
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, PUT, POST");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
  
  // serve up the index.html file
  server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server->on("/update", HTTP_POST, [](AsyncWebServerRequest *request){

    if(request->hasParam("ssid", true)){
      AsyncWebParameter* p = request->getParam("ssid", true);
      String ssidValue = p->value();
      if(request->hasParam("pass", true)){
        String passValue = request->getParam("pass", true) -> value();
        Serial.printf("received request to update ssid and pass -> %s : %s \r\n", ssidValue, passValue);
        //
        // write new values to EPROM
        //
        FlashData data;
        EEPROM.begin(EPROM_SIZE);
        EEPROM.get(0, data);
        strcpy(data.ssid, ssidValue.c_str());
        strcpy(data.password, passValue.c_str());
        EEPROM.put(0, data);
        EEPROM.commit();
      }
    }

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
    response->addHeader("Connection", "close");
    request->send(response);
  });


  // setup ws server
  ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
  {
    if (type == WS_EVT_CONNECT)
    {
      Serial.printf("ws[%s][%u] connect\r\n", server->url(), client->id());
      this->connected_client = client;
    }
    else if (type == WS_EVT_DISCONNECT)
    {
      Serial.printf("ws[%s][%u] disconnect\r\n", server->url(), client->id());
      this->connected_client = NULL;
    }
    else if (type == WS_EVT_ERROR)
    {
      Serial.printf("ws[%s][%u] error(%u): %s\r\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
    }
  });
  server->addHandler(ws);
  // handle options requests for cors or not found
  server->onNotFound([](AsyncWebServerRequest *request)
  {
    if (request->method() == HTTP_OPTIONS)
    {
      request->send(200);
    }
    else
    {
      Serial.println("Not found");
      request->send(404, "Not found");
    }
  });

  server->begin();
}

void WebSocketTransport::send(void *data, size_t len)
{
  // This seems to have a lot of issues...
  // ws->binaryAll((uint8_t *)data, len);
  // so we'll stick to just the one client...
  if (connected_client && connected_client->canSend())
  {
    connected_client->binary(reinterpret_cast<uint8_t *>(data), len);
  }
}