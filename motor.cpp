/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#include "motor.h"
#include "settings.h"
#include "logger.h"

#include <Arduino.h>

#define ZU_MS    -10000
#define AUF_MS    60000
#define KALIB_MS 999999

Motor::Motor(Ort ort)
  : ort(ort), richtung(STOP), status(STOP)
{
  lastpos = -1;
  fullms = 20000;
  posms = fullms;
}

/**
 * @brief Motor::moveto
 *
 * Bewege Motor zu angegebener Position.
 * Diese Funktion setzt nur zielms und fuehrt sonst keine Veraenderung durch.
 * Die Zuweisung des aktuell zu bewegenden Motors geschieht in update().
 * Die State Machine des Motors wird in move() bearbeitet.
 *
 * @param pos Position (0 .. 100)
 */
void Motor::moveto(int pos)
{
  if (pos == lastpos) {
    return;
  }

  lastpos = pos;

  Log.print("Motor ");
  Log.print(ort);
  Log.print(" moveto(");
  Log.print(pos);
  Log.print(") Altes Ziel=");
  Log.print(zielms);

  fullms = Settings::settings.getFullMs(ort);
  long z = fullms * pos / 100;

  // fuer Anschlagpositionen Ziel weit darueber hinaus festlegen
  if (z == 0) {
    z = ZU_MS;
  } else if (z == fullms) {
    z = AUF_MS;
  }

  if (z != zielms) {
    Log.print(" Neues Ziel=");
    Log.print(z);
    zielms = z;
  }

  Log.println();
}


/**
 * @brief Motor::update
 *
 * Bestimmt Richtungswunsch aus aktueller Position und Ziel.
 * Diese Funktion setzt nur richtung und fuehrt sonst keine Veraenderung durch.
 * Die Richtung dient als Event fuer die State Machine, die in move() bearbeitet wird.
 *
 * @return true wenn Motor bewegt werden soll
 */
bool Motor::update()
{
  if (posms < zielms - 300) {
    // aufmachen
    richtung = AUF;

    if (!moving) {
      Log.println("Starte Motor auf");
//      starttime = millis();
//      startms = posms;
      moving = this;
    }

    return true;
  } else if (posms > zielms + 300) {
    // zumachen
    richtung = ZU;

    if (!moving) {
      Log.println("Starte Motor zu");
//      starttime = millis();
//      startms = posms;
      moving = this;
    }

    return true;
  } else {
    richtung = STOP;
    return false;
  }
}

void Motor::kalibrieren()
{
  /*
   * Erst ganz zumachen, dann ganz oeffnen und Millisekunden zaehlen
   * Wenn kein Strom mehr fliesst, dann muss der Anschlag erreicht sein
   */
  posms = KALIB_MS;
  zielms = 0;

  do {
    update();
    delay(10);
  } while (move());

  posms = 0;
  fullms = KALIB_MS;
  zielms = KALIB_MS;

  do {
    update();
    delay(10);
  } while (move());

  fullms = posms;
  Log.print("fullms=");
  Log.println(fullms);
  Settings::settings.setFullMs(ort, fullms);
}


int Motor::pos()
{
  // begrenze posms
  if (posms < 0) {
    posms = 0;
  } else if (posms > fullms) {
    posms = fullms;
  }

  return map(posms, 0, fullms, 0, 100);
}

/**
 * @brief Motor::move
 *
 * State Machine fuer Motor
 *
 * @return
 */
bool Motor::move()
{
  static int cnt = 0;

  if (moving) {
    if ((moving->status == AUF || moving->status == ZU)
        && millis() - moving->starttime > 500
        && analogRead(MOTOR_STROM) < 30) {
      // Strom < 100mA -> ist am Anschlag, stoppen
      Log.println("Anschlag erreicht");
      // Kalibrierung wiederherstellen
      if (moving->fullms != KALIB_MS) {
        moving->posms = moving->status == ZU ? 0 : moving->fullms;
      }

      moving->zielms = moving->posms;
      moving->richtung = STOP;
    }

    if (++cnt % 100 == 0) {
      Log.print("Analogwert ");
      Log.println(analogRead(MOTOR_STROM));
    }

    switch (moving->status) {
    case STOP:
      digitalWrite(FENSTER_HOBBY, RELAY_OFF);
      digitalWrite(FENSTER_AUF, RELAY_OFF);
      digitalWrite(FENSTER_ZU, RELAY_OFF);
      digitalWrite(MOTOR_AN, RELAY_OFF);

      if (moving->richtung == AUF) {
        Log.println("Motor Status TRANS_AUF");
        moving->status = TRANS_AUF;
        moving->starttime = millis();
      } else if (moving->richtung == ZU) {
        Log.println("Motor Status TRANS_ZU");
        moving->status = TRANS_ZU;
        moving->starttime = millis();
      } else {
        moving = nullptr;
      }
      break;

    case TRANS_AUF:
      digitalWrite(FENSTER_HOBBY, moving->ort == HOBBY ? RELAY_ON : RELAY_OFF);
      digitalWrite(FENSTER_AUF, RELAY_ON);
      digitalWrite(FENSTER_ZU, RELAY_OFF);
      digitalWrite(MOTOR_AN, RELAY_OFF);

      if (moving->richtung == AUF) {
        if (millis() - moving->starttime > 500) {
          Log.println("Motor Status AUF");
          moving->status = AUF;
          moving->startms = moving->posms;
          moving->starttime = millis();
        }
      } else if (moving->richtung == ZU) {
        if (millis() - moving->starttime > 500) {
          Log.println("Motor Status TRANS_ZU");
          moving->status = TRANS_ZU;
          moving->starttime = millis();
        }
      } else {
        if (millis() - moving->starttime > 500) {
          Log.println("Motor Status STOP");
          moving->status = STOP;
        }
      }
      break;

    case AUF:
      digitalWrite(FENSTER_HOBBY, moving->ort == HOBBY ? RELAY_ON : RELAY_OFF);
      digitalWrite(FENSTER_AUF, RELAY_ON);
      digitalWrite(FENSTER_ZU, RELAY_OFF);
      digitalWrite(MOTOR_AN, RELAY_ON);

      if (moving->richtung == AUF) {
        moving->posms = moving->startms + (millis() - moving->starttime);
      } else {
        Log.println("Motor Status TRANS_AUF");
        moving->status = TRANS_AUF;
        moving->starttime = millis();
      }
      break;

    case TRANS_ZU:
      digitalWrite(FENSTER_HOBBY, moving->ort == HOBBY ? RELAY_ON : RELAY_OFF);
      digitalWrite(FENSTER_AUF, RELAY_OFF);
      digitalWrite(FENSTER_ZU, RELAY_ON);
      digitalWrite(MOTOR_AN, RELAY_OFF);

      if (moving->richtung == AUF) {
        if (millis() - moving->starttime > 500) {
          Log.println("Motor Status TRANS_AUF");
          moving->status = TRANS_AUF;
          moving->starttime = millis();
        }
      } else if (moving->richtung == ZU) {
        if (millis() - moving->starttime > 500) {
          Log.println("Motor Status ZU");
          moving->status = ZU;
          moving->startms = moving->posms;
          moving->starttime = millis();
        }
      } else {
        if (millis() - moving->starttime > 500) {
          Log.println("Motor Status STOP");
          moving->status = STOP;
        }
      }
      break;

    case ZU:
      digitalWrite(FENSTER_HOBBY, moving->ort == HOBBY ? RELAY_ON : RELAY_OFF);
      digitalWrite(FENSTER_AUF, RELAY_OFF);
      digitalWrite(FENSTER_ZU, RELAY_ON);
      digitalWrite(MOTOR_AN, RELAY_ON);

      if (moving->richtung == ZU) {
        moving->posms = moving->startms - (millis() - moving->starttime);
      } else {
        Log.println("Motor Status TRANS_ZU");
        moving->status = TRANS_ZU;
        moving->starttime = millis();
      }
      break;
    }
  } else {
    digitalWrite(FENSTER_HOBBY, RELAY_OFF);
    digitalWrite(FENSTER_AUF, RELAY_OFF);
    digitalWrite(FENSTER_ZU, RELAY_OFF);
    digitalWrite(MOTOR_AN, RELAY_OFF);
  }

  return moving;
}


Motor* Motor::moving = nullptr;


const char *Motor::toString(Richtung r)
{
  static const char* rtext [] = {
    "STOP",
    "AUF",
    "ZU",
    "TRANS_AUF",
    "TRANS_ZU"
  };

  return rtext[r];
}
