/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#ifndef MOTOR_H
#define MOTOR_H

#include "enums.h"


class Motor
{
public:
  Motor(Ort ort);

  void moveto(int pos); // 0 (zu) bis 100 (offen)
  bool update();
  void kalibrieren();
  int pos();
  bool blocked(bool keep = true);
  static bool move();
  static const char* toString(Richtung r);

public:
  Ort ort;
  Richtung richtung; // Zielrichtung (Wunschrichtung)
  Richtung status;   // Aktueller Status (State Machine)

private:
  int lastpos;
  long zielms;
  long posms;
  long startms;
  unsigned long starttime;
  long fullms; // Millisekunden fuer Position 100, ganz offen

  static Motor* moving;
};

#endif // MOTOR_H
