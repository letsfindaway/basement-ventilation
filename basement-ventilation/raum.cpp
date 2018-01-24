/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#include "raum.h"
#include "settings.h"
#include "logger.h"

Raum::Raum(Ort ort, Klima &innen, Klima &aussen)
  : ort(ort), innen(innen), aussen(aussen), motor(ort)
{
  modus = AUTOMATISCH;
  lastpos = -1;
}

void Raum::setup()
{
  motor.kalibrieren();
}

/**
 * @brief Raum::update
 *
 * Entscheidungen treffen in folgender Reihenfolge:
 * - manuell? Dann keine Aenderung
 * - zu kalt? Dann zumachen
 * - Draussen trockener als innen? Dann aufmachen
 * - Heizung an? Dann halb aufmachen
 * - Draussen feuchter als innen? Dann zumachen
 */
bool Raum::update()
{
  Modus m = modus;

  // manuell? Dann keine Aenderung
  if (modus == MANUELL) {
    m = MANUELL;
  }

  // zu kalt? Dann zumachen
  else if (innen.temp < Settings::settings.getMinTemp(ort)) {
    m = TEMPERATUR;
    motor.moveto(0);
  }

  // Draussen trockener als innen und nicht zu kalt? Dann aufmachen
  else if (aussen.abshum < innen.abshum - Settings::settings.getMaxDiff(ort)
           && innen.temp >= Settings::settings.getMinTemp(ort) + 0.5) {
    m = FEUCHT;
    motor.moveto(100);
  }

  // Heizung an und nicht zu kalt? Dann halb aufmachen
  else if (ort == HOBBY && digitalRead(ABGAS) == LOW
           && innen.temp >= Settings::settings.getMinTemp(ort) + 0.3) {
    m = HEIZUNG;
    motor.moveto((int)Settings::settings.getHeizWin());
  }

  // Draussen feuchter als innen? Dann zumachen
  else if (aussen.abshum > innen.abshum - Settings::settings.getMinDiff(ort)) {
    m = TROCKEN;
    motor.moveto(0);
  }

  motor.update();

  if (m != modus) {
    Log.print(ort == HOBBY ? "Hobby " : "Keller ");
    Log.print("Modus=");
    Log.println(toString(m));
    modus = m;
    return true;
  }

  return false;
}


bool Raum::moved(int &pos)
{
  pos = motor.pos();

  if (pos != lastpos) {
    lastpos = pos;
    return true;
  }

  return false;
}


void Raum::fenster(Richtung richtung)
{
  if (modus != MANUELL) {
    return;
  }

  Log.print("Fenster ");
  Log.println(Motor::toString(richtung));

  switch (richtung) {
  case AUF:
    motor.moveto(100);
    break;

  case ZU:
    motor.moveto(0);
    break;

  case STOP:
    motor.moveto(motor.pos());
    break;
  }
}


const char *Raum::toString(Modus modus)
{
  static const char* mtext[] = {
    "MANUELL",
    "AUTOMATISCH",
    "FEUCHT",
    "TROCKEN",
    "HEIZUNG",
    "TEMPERATUR"
  };

  return mtext[modus];
}
