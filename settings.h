#ifndef SETTINGS_H
#define SETTINGS_H

#include "pins.h"
class Settings
{
public:
  Settings();

private:
  // Sensoren
  float kellerToffset;
  float kellerTfaktor;
  float kellerHoffset;
  float kellerHfaktor;

  float hobbyToffset;
  float hobbyTfaktor;
  float hobbyHoffset;
  float hobbyHfaktor;

  float aussenToffset;
  float aussenTfaktor;
  float aussenHoffset;
  float aussenHfaktor;

  // Aktoren
  int kellerFullMs;
  int hobbyFullMs;

  // Raumeinstellungen
  float kellerMinDiff;
  float kellerMaxDiff;
  float kellerMinTemp;

  float hobbyMinDiff;
  float hobbyMaxDiff;
  float hobbyMinTemp;
  float hobbyHeizWin;

public:
  static Settings settings;

  void load();
  void save();

  // Getter
  float getToffset(Ort ort);
  float getTfaktor(Ort ort);
  float getHoffset(Ort ort);
  float getHfaktor(Ort ort);
  int getFullMs(Ort ort);
  float getMinDiff(Ort ort);
  float getMaxDiff(Ort ort);
  float getMinTemp(Ort ort);
  float getHeizWin();

  // Setter
  void setToffset(Ort ort, float o);
  void setTfaktor(Ort ort, float f);
  void setHoffset(Ort ort, float o);
  void setHfaktor(Ort ort, float f);
  void setFullMs(Ort ort, int t);
  void setMinDiff(Ort ort, float f);
  void setMaxDiff(Ort ort, float f);
  void setMinTemp(Ort ort, float f);
  void setHeizWin(float f);

private:
  // Utility
  float round1(float f);
};

#endif // SETTINGS_H
