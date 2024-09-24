#pragma once

#include <cstddef>


class Transport;
class I2SSampler;

class Application
{
private:
  Transport *transport2 = NULL;
  I2SSampler *input = NULL;
  I2SSampler *input2 = NULL;

public:
  void begin();
  static void streamer_task(void *param);
  static void streamer_task2(void *param);
};