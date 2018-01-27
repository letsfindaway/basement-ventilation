/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#include "raum.h"
#include "customize.h"
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
 * Decisions are made in the following order:
 * - manual? Then no action
 * - too cold? Then close window
 * - outside less humid than inside and temperature high enough? Then open window
 * - heating on and not too cold? Then open window a little bit
 * - outside more humid than inside? Then close window
 */
bool Raum::update()
{
  Modus m = modus;

  // manual? Then no action
  if (modus == MANUELL) {
    m = MANUELL;
  }

  // too cold? Then close window
  else if (innen.temp < Settings::settings.getMinTemp(ort)) {
    m = TEMPERATUR;
    motor.moveto(0);
  }

  // outside less humid than inside and temperature high enough? Then open window
  else if (aussen.abshum < innen.abshum - Settings::settings.getMaxDiff(ort)
           && innen.temp >= Settings::settings.getMinTemp(ort) + 0.5) {
    m = FEUCHT;
    motor.moveto(100);
  }

  // heating on and not too cold? Then open window a little bit
  else if (ort == HOBBY && digitalRead(ABGAS) == LOW
           && innen.temp >= Settings::settings.getMinTemp(ort) + 0.3) {
    m = HEIZUNG;
    motor.moveto((int)Settings::settings.getHeizWin());
  }

  // outside more humid than inside? Then close window
  else if (aussen.abshum > innen.abshum - Settings::settings.getMinDiff(ort)) {
    m = TROCKEN;
    motor.moveto(0);
  }

  // now perform the update on the motor
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


/**
 * @brief Raum::moved
 *
 * Check whether motor position changed and return actual position
 *
 * @param pos returns the actual position of the window in percent (0..100)
 * @return true if position changed since last call, false otherwise
 */
bool Raum::moved(int &pos)
{
  pos = motor.pos();

  if (pos != lastpos) {
    lastpos = pos;
    return true;
  }

  return false;
}


/**
 * @brief Raum::fenster
 *
 * Instruct the window to open, close or stop. Useful for manual operation.
 *
 * @param richtung Direction AUF, ZU or STOP
 */
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
