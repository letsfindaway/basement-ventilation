#ifndef WLAN_H
#define WLAN_H

//#define LIMITED_RAM_DEVICE
#include "klima.h"
#include "raum.h"

#include <WiFi101.h>
#include <SD.h>
#include "TimeLib.h"

class Wlan
{
public:
  Wlan();
  bool connect();
  time_t getNtpTime();
  void serve(Klima &k1, Klima &k2, Klima &k3, Raum &r1, Raum &r2);

private:
  void sendNTPpacket(IPAddress& address);
  void sendSVG(Klima &k1, Klima &k2, Klima &k3, Raum &r1, Raum &r2);
  void sendSVGKlima(Klima& k, int y);
  void sendSVGRaum(Raum& r, int y);
  unsigned long lastRequest;

  enum Phase {
    IDLE,
    SKIP_HEADER,
    RESPOND,
    RESPONDSVG,
    SEND_DATA
  } phase;

  String req;
  bool currentLineIsBlank;
  int tag;
  File daten;
};

#endif // WLAN_H
