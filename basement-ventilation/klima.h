/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#ifndef KLIMA_H
#define KLIMA_H

// Verwaltung und Management eines DHT22 Sensors
#include "pins.h"

#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>

class Klima {
  public:
    Klima(Ort ort, int pin);
    void setup(unsigned long when, unsigned long every);
    bool update();

  private:
    Ort ort;
    DHT dht;
    long next;
    long every;

  public:
    float temp;
    float humidity;
    float abshum;
};

#endif
