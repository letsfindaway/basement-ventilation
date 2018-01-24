/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#include "klima.h"
#include "settings.h"
#include "logger.h"

Klima::Klima(Ort ort, int pin)
  : ort(ort), dht(pin, DHT22), temp(0), humidity(0), abshum(0)
{

}

void Klima::setup(unsigned long when, unsigned long every)
{
  dht.begin();
  next = millis() + 1000 * when;
  this->every = 1000 * every;
}

bool Klima::update() {
  if ((long)millis() - next < 0) {
    return false;
  }
  
  float h = (dht.readHumidity() + Settings::settings.getHoffset(ort)) * Settings::settings.getHfaktor(ort);
  float t = (dht.readTemperature() + Settings::settings.getToffset(ort)) * Settings::settings.getTfaktor(ort);
  
  if (isnan(h) || isnan(t)) {
    next += every;
    return false;
  }

  humidity = h;
  temp = t;
  // nach http://www.pb-schilling.de/baubiologie/luftfeuchte-rechner/
  // Formeln nach DIN 4108
  // abshum liegt so etwa zwischen 0 und 20, wobei Werte ueber 12 eher feucht sind
  float wasserdampfSaettingungsdruck = t > 0 ? 288.68 * pow(1.098+t/100, 8.02) : 4.689 * pow(1.486+t, 12.3);
  float wasserdampfTeildruck = h/100 * wasserdampfSaettingungsdruck;
  abshum = 216.7 * wasserdampfTeildruck * 0.01 / (temp + 273.15);

  Log.print("Sensor ");
  Log.print(ort);
  Log.print(" ");
  Log.print(t);
  Log.print("C ");
  Log.print(h);
  Log.print("% ");
  Log.print(abshum);
  Log.println("g");

  next += every;
  return true;
}

