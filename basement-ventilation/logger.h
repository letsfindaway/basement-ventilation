/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "Print.h"
#include <WiFiUdp.h>

class Logger : public Print
{
public:
  Logger();
  bool begin();

  virtual size_t write(uint8_t c);
  virtual size_t write(const uint8_t *buffer, size_t size);

private:
  String line;
  WiFiUDP udp;
  IPAddress addr;
};

extern Logger Log;

#endif // LOGGER_H
