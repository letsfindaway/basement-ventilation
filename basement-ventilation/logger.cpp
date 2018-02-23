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
    writeLine();

    // reset line
    line.remove(0);
  }

  return 1;
}


size_t Logger::write(const uint8_t *buffer, size_t size)
{
  // Implementation of Print guarantees that the buffer contains a null
  // terminated string, so we can just ignore the size.
  line.concat((const char*)buffer);
  size_t len = line.length();

  if (line[len - 1] == '\n') {
    // line end, output line
    writeLine();

    // reset line
    line.remove(0);
  }

  return len;
}

void Logger::writeLine()
{
  // log to serial if initialized
  if (Serial) {
    Serial.print(line);
  }

  if (line.length() <= 2) {
    // don't log empty lines to network
    return;
  }

  // try up to three times to send UDP packet
  for (int i = 0; i < 3; ++i) {
    if (udp.beginPacket(addr, BV_LOG_PORT)) {
      udp.write(line.c_str(), line.length() - 2);

      if (udp.endPacket()) {
        // everything was successful
        delay(SPI_HOLDOFF_TIMEMS);
        return;
      }
    }

    // a problem occured, reinitialize the udp
    udp.stop();
    udp.begin(2391);
  }
}
