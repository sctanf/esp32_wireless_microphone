#pragma once

class Transport
{
public:
  virtual void begin() = 0;
  virtual void send(void *data, size_t bytes) = 0;
  virtual size_t read(void *buffer, size_t bytes) = 0;
};