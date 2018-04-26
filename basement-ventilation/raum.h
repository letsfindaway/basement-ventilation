/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#ifndef RAUM_H
#define RAUM_H

#include "enums.h"
#include "klima.h"
#include "motor.h"

/**
 * @brief The Raum class represents a room with a window operated by a motor.
 * It references two Klima object, one for the outside climate and one for the
 * inside climate. Based on this data it decides on the mode and window position.
 */
class Raum
{
public:
  Raum(Ort ort, Klima& innen, Klima& aussen);
  void setup();
  bool update();
  bool moved(int& pos);
  void fenster(Richtung richtung);
  void setModus(Modus m);
  void interrupt();
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
