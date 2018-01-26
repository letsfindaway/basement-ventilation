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
  
  // prepare for the next time
  next += every;

  /*
   * Here we provide the option to calibrate the sensor values by using an
   * offset and factor. However until now I did not make use of this feature.
   * The factor is initialized to 1.0 and the offset to 0.0 within Settings.
   */
  float h = (dht.readHumidity() + Settings::settings.getHoffset(ort)) * Settings::settings.getHfaktor(ort);
  float t = (dht.readTemperature() + Settings::settings.getToffset(ort)) * Settings::settings.getTfaktor(ort);
  
  // Reading unsuccessful? Just try it again next time
  if (isnan(h) || isnan(t)) {
    return false;
  }

  humidity = h;
  temp = t;
  // see also http://www.pb-schilling.de/baubiologie/luftfeuchte-rechner/
  // formula according to DIN 4108
  // values are about between 0 and 20, where values above 12 are quite humid
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

  return true;
}
