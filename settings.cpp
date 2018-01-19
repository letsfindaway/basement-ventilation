#include "settings.h"
#include "logger.h"

#include <SD.h>

Settings Settings::settings;

Settings::Settings()
{
  kellerToffset = 0;
  kellerTfaktor = 1;
  kellerHoffset = 0;
  kellerHfaktor = 1;

  hobbyToffset = 0;
  hobbyTfaktor = 1;
  hobbyHoffset = 0;
  hobbyHfaktor = 1;

  aussenToffset = 0;
  aussenTfaktor = 1;
  aussenHoffset = 0;
  aussenHfaktor = 1;

  // Aktoren
  kellerFullMs = 1000;
  hobbyFullMs = 1000;

  // Raumeinstellungen
  kellerMinDiff = 1;
  kellerMaxDiff = 2;
  kellerMinTemp = 14;

  hobbyMinDiff = 1;
  hobbyMaxDiff = 2;
  hobbyMinTemp = 17;
  hobbyHeizWin = 25;
}


float Settings::getToffset(Ort ort)
{
  switch (ort) {
  case KELLER: return kellerToffset;
  case HOBBY:  return hobbyToffset;
  case AUSSEN: return aussenToffset;
  }
}


float Settings::getTfaktor(Ort ort)
{
  switch (ort) {
  case KELLER: return kellerTfaktor;
  case HOBBY:  return hobbyTfaktor;
  case AUSSEN: return aussenTfaktor;
  }
}

float Settings::getHoffset(Ort ort)
{
  switch (ort) {
  case KELLER: return kellerHoffset;
  case HOBBY:  return hobbyHoffset;
  case AUSSEN: return aussenHoffset;
  }
}

float Settings::getHfaktor(Ort ort)
{
  switch (ort) {
  case KELLER: return kellerHfaktor;
  case HOBBY:  return hobbyHfaktor;
  case AUSSEN: return aussenHfaktor;
  }
}

int Settings::getFullMs(Ort ort)
{
  switch (ort) {
  case KELLER: return kellerFullMs;
  case HOBBY:  return hobbyFullMs;
  case AUSSEN: return 0;
  }
}

float Settings::getMinDiff(Ort ort)
{
  switch (ort) {
  case KELLER: return kellerMinDiff;
  case HOBBY:  return hobbyMinDiff;
  case AUSSEN: return 0;
  }
}

float Settings::getMaxDiff(Ort ort)
{
  switch (ort) {
  case KELLER: return kellerMaxDiff;
  case HOBBY:  return hobbyMaxDiff;
  case AUSSEN: return 0;
  }
}

float Settings::getMinTemp(Ort ort)
{
  switch (ort) {
  case KELLER: return kellerMinTemp;
  case HOBBY:  return hobbyMinTemp;
  case AUSSEN: return 0;
  }
}

float Settings::getHeizWin()
{
  return hobbyHeizWin;
}


void Settings::setToffset(Ort ort, float o)
{
  switch (ort) {
  case KELLER: kellerToffset = o; break;
  case HOBBY:  hobbyToffset = o; break;
  case AUSSEN: aussenToffset = o; break;
  }
}

void Settings::setTfaktor(Ort ort, float f)
{
  switch (ort) {
  case KELLER: kellerTfaktor = f; break;
  case HOBBY:  hobbyTfaktor = f; break;
  case AUSSEN: aussenTfaktor = f; break;
  }
}

void Settings::setHoffset(Ort ort, float o)
{
  switch (ort) {
  case KELLER: kellerHoffset = o; break;
  case HOBBY:  hobbyHoffset = o; break;
  case AUSSEN: aussenHoffset = o; break;
  }
}

void Settings::setHfaktor(Ort ort, float f)
{
  switch (ort) {
  case KELLER: kellerHfaktor = f; break;
  case HOBBY:  hobbyHfaktor = f; break;
  case AUSSEN: aussenHfaktor = f; break;
  }
}

void Settings::setFullMs(Ort ort, int t)
{
  switch (ort) {
  case KELLER: kellerFullMs = t; break;
  case HOBBY:  hobbyFullMs = t; break;
  case AUSSEN: break;
  }
}

void Settings::setMinDiff(Ort ort, float f)
{
  switch (ort) {
  case KELLER: kellerMinDiff = round1(f); break;
  case HOBBY:  hobbyMinDiff = round1(f); break;
  case AUSSEN: break;
  }
}

void Settings::setMaxDiff(Ort ort, float f)
{
  switch (ort) {
  case KELLER: kellerMaxDiff = round1(f); break;
  case HOBBY:  hobbyMaxDiff = round1(f); break;
  case AUSSEN: break;
  }
}

void Settings::setMinTemp(Ort ort, float f)
{
  switch (ort) {
  case KELLER: kellerMinTemp = round1(f); break;
  case HOBBY:  hobbyMinTemp = round1(f); break;
  case AUSSEN: break;
  }
}

void Settings::setHeizWin(float f)
{
  hobbyHeizWin = constrain(f, 0, 100);
}


void Settings::load()
{
  File file = SD.open("/settings.dat");

  if (!file) {
    Log.println("settings.dat nicht gefunden");
    return;
  }

  file.read(this, sizeof(Settings));
  file.close();
  Log.println("Settings geladen");
}


void Settings::save()
{
  File file = SD.open("/settings.dat", FILE_WRITE);

  if (!file) {
    Log.println("Kann settings.dat nicht zum Schreiben oeffnen");
    return;
  }

  file.seek(0);
  file.write((const uint8_t*)this, sizeof(Settings));
  file.close();
  Log.println("Settings gespeichert");
}


float Settings::round1(float f)
{
  int val = (int)(10 * f + 0.5f);
  return (float)val / 10;
}
