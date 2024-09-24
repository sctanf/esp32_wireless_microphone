#include <WiFiServer.h>
#include "TCPSocketTransport.h"

void TCPSocketTransport::begin()
{
  server = new WiFiServer(9090);
  server->begin();
}

void TCPSocketTransport::send(void *data, size_t len)
{
  // get any new connections
  WiFiClient client = server->available();
  if (client)
  {
    Serial.println("New Client");
    // add to the list of clients
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
      if (NULL == clients[i])
      {
        clients[i] = new WiFiClient(client);
        break;
      }
    }
  }
  // send the audio data to any clients
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if (clients[i] != NULL && (*clients[i]))
    {
      // send the samples to the client
      clients[i]->write((uint8_t *)data, len);
    }
    else
    {
      // client has gone away, remove it from the list
      clients[i] = NULL;
    }
  }
}

size_t TCPSocketTransport::read(void *buffer, size_t bytes)
{
  if (clients[0] != NULL)
    // get audio data from first client
    return clients[0]->read((uint8_t *)buffer, bytes);
  else
    return 0;
}