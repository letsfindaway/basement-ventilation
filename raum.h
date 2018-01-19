#ifndef RAUM_H
#define RAUM_H

#include "klima.h"
#include "motor.h"

enum Modus {
  MANUELL,
  AUTOMATISCH,
  FEUCHT,
  TROCKEN,
  HEIZUNG,
  TEMPERATUR
};

class Raum
{
public:
  Raum(Ort ort, Klima& innen, Klima& aussen);
  void setup();
  bool update();
  bool moved(int& pos);
  void fenster(Richtung richtung);
  static const char* toString(Modus modus);

public:
  Modus modus;
  int lastpos;

private:
  Ort ort;
  Klima& innen;
  Klima& aussen;
  Motor  motor;
};

#endif // RAUM_H
