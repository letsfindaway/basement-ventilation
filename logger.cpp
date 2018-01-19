#include "logger.h"
#include <Arduino.h>

// globale Instanz
Logger Log;

Logger::Logger()
{
}

bool Logger::begin()
{
  addr = IPAddress(192, 168, 178, 26);
  return udp.begin(2391);
}


size_t Logger::write(uint8_t c)
{
  line += (char)c;

  if (c == '\n') {
    // Zeilenende, ausgeben!
    Serial.print(line);

    if (line.length() > 2) {
      udp.beginPacket(addr, 4242);
      udp.write(line.c_str(), line.length() - 2);
      udp.endPacket();
    }

    // Zeile zuruecksetzen
    line.remove(0);
  }

  return 1;
}


size_t Logger::write(const uint8_t *buffer, size_t size)
{
  line.concat((const char*)buffer);
  size_t len = line.length();

  if (line[len - 1] == '\n') {
    // Zeilenende, ausgeben!
    Serial.print(line);

    if (len > 2) {
      udp.beginPacket(addr, 4242);
      udp.write(line.c_str(), len - 2);
      udp.endPacket();
    }

    // Zeile zuruecksetzen
    line.remove(0);
  }

  return len;
}
