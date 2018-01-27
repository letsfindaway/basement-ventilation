/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#ifndef ANZEIGE_H
#define ANZEIGE_H

#include "klima.h"
#include "raum.h"


#include <SPI.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_HX8357.h>
#include <Adafruit_STMPE610.h>
#include <SD.h>


class Anzeige {
public:
  enum Event {
    EVT_NULL,
    EVT_MODUS_KELLER,
    EVT_MODUS_HOBBY,
    EVT_AUF_KELLER,
    EVT_AUF_HOBBY,
    EVT_ZU_KELLER,
    EVT_ZU_HOBBY,
    EVT_STOP_KELLER,
    EVT_STOP_HOBBY,
    EVT_CONFIG_KELLER,
    EVT_CONFIG_HOBBY,
    EVT_CFG_HOME,
    EVT_CFG_MIND_UP,
    EVT_CFG_MIND_DN,
    EVT_CFG_MAXD_UP,
    EVT_CFG_MAXD_DN,
    EVT_CFG_MINT_UP,
    EVT_CFG_MINT_DN,
    EVT_CFG_WIN_UP,
    EVT_CFG_WIN_DN
  };

  enum Screen {
    HOME,
    CONFIG
  };

  Anzeige();
  void setup();
  void screen(Screen scr, Ort ort = AUSSEN);
  void printKlima(Ort ort, Klima& klima);
  void printModus(Ort ort, Modus modus);
  void printFenster(Ort ort, int pos);
  void printCfg(float mind, float maxd, float mint, float win = NAN);
  Event getEvent();
  void log(Klima& k1, Klima& k2, Klima& k3, Raum& r1, Raum& r2);
  void startlog();

private:
  void bmpDraw(const char *filename, uint8_t x, uint16_t y);
  char* fmt(char* s, float nbr, int digits, bool decimal = false, bool sign = false);
  
private:
  Adafruit_HX8357 tft;
  Adafruit_STMPE610 ts;
  Screen current;
  Event lastevent;
  long lasttime;
  int nextminute;
};

#endif

