#include <WiFiServer.h>
#include "TCPSocketTransport.h"

void TCPSocketTransport::begin()
{
  server = new WiFiServer(9090);
  server->begin();
  server->setNoDelay(true);
  server->setTimeout(1);
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
        clients[i]->setTimeout(1);
        int opt = 1;
        int val = 4096;
        clients[i]->setSocketOption(SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
        clients[i]->setSocketOption(SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        clients[i]->setSocketOption(SOL_SOCKET, SO_RCVBUF, &val, sizeof(val));
        clients[i]->setNoDelay(true);
        Serial.println("New Client");
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
  if (clients[0] != NULL && clients[0]->available())
  {
    // get audio data from first client
//    if (clients[0]->available() > bytes * 10)
//    {
//      clients[0]->read((uint8_t *)buffer, bytes);
//    }
    size_t bytesRead = clients[0]->read((uint8_t *)buffer, bytes);
    return bytesRead;
  }
  else
    return 0;
}