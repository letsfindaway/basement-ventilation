/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#include "logger.h"
#include "customize.h"
#include <Arduino.h>

// global instance
Logger Log;

Logger::Logger()
{
}

bool Logger::begin()
{
  addr = IPAddress(BV_LOG_TARGET); // my desktopp computer
  return udp.begin(2391);
}


size_t Logger::write(uint8_t c)
{
  line += (char)c;

  if (c == '\n') {
    // line end, output line
    if (Serial) {
      Serial.print(line);
    }

    if (line.length() > 2) {
      udp.beginPacket(addr, BV_LOG_PORT);
      udp.write(line.c_str(), line.length() - 2);
      udp.endPacket();
      delay(SPI_HOLDOFF_TIMEMS);
    }

    // reset line
    line.remove(0);
  }

  return 1;
}


size_t Logger::write(const uint8_t *buffer, size_t size)
{
  line.concat((const char*)buffer);
  size_t len = line.length();

  if (line[len - 1] == '\n') {
    // line end, output line
    if (Serial) {
      Serial.print(line);
    }

    if (len > 2) {
      udp.beginPacket(addr, BV_LOG_PORT);
      udp.write(line.c_str(), len - 2);
      udp.endPacket();
      delay(SPI_HOLDOFF_TIMEMS);
    }

    // reset line
    line.remove(0);
  }

  return len;
}
