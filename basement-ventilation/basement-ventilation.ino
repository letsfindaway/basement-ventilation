/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#include "customize.h"
#include "klima.h"
#include "anzeige.h"
#include "raum.h"
#include "motor.h"
#include "settings.h"
#include "wlan.h"
#include "logger.h"

#include "TimeLib.h"

// define the objects used by the sketch
Klima aussen(AUSSEN, DHT22_AUSSEN);
Klima keller(KELLER, DHT22_KELLER);
Klima hobby (HOBBY, DHT22_HOBBY);
Anzeige anzeige;
Raum kellerraum(KELLER, keller, aussen);
Raum hobbyraum(HOBBY, hobby, aussen);
Wlan wlan;

// simple utility to compute free RAM
extern "C" char *sbrk(int i);

int FreeStorage () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

void setup() {
  // configure pins before doing anything else
  pinMode(ABGAS,         INPUT_PULLUP);
  pinMode(FENSTER_AUF,   OUTPUT);
  pinMode(FENSTER_ZU,    OUTPUT);
  pinMode(FENSTER_HOBBY, OUTPUT);
  pinMode(MOTOR_AN,      OUTPUT);
  pinMode(MOTOR_STROM,   INPUT);
  
  digitalWrite(FENSTER_AUF,   RELAY_OFF);
  digitalWrite(FENSTER_ZU,    RELAY_OFF);
  digitalWrite(FENSTER_HOBBY, RELAY_OFF);
  digitalWrite(MOTOR_AN,      RELAY_OFF);

  analogReadResolution(10);

  // try to initialize Serial for 5 seconds
  Serial.begin(9600);
  int cnt = 50;
  while (!Serial && cnt--) delay(100);

  Serial.println("Basement Ventilation");
  Serial.print("Free RAM: ");
  Serial.println(FreeStorage());

  Serial.println("Initialize display");
  anzeige.setup();
  anzeige.screen(Anzeige::HOME);

  Serial.println("Initialize WiFi");
  wlan.connect();

  // now the Logger can be used, we continue with setup of sensors and motors
  Log.begin();
  Log.println("Start UDP Logger");
  Log.println("Initialize Sensors");
  Settings::settings.load();

  aussen.setup(0, 30);
  keller.setup(10, 30);
  hobby.setup(20, 30);

  Serial.print("Free RAM: ");
  Log.println(FreeStorage());

  Log.println("Calibrate motors");
  kellerraum.setup();
  hobbyraum.setup();
  Settings::settings.save();

  Log.println("Close windows");
  kellerraum.fenster(ZU);
  hobbyraum.fenster(ZU);

  wlan.getNtpTime();
  Log.println("Setup finished");
}

void loop() {
  static Ort cfgOrt; // used in configuration mode to remember which room is configured
  Settings& s(Settings::settings); // just a shortcut

  // read sensors
  if (aussen.update()) {
    anzeige.printKlima(AUSSEN, aussen);
  }

  if (hobby.update()) {
    anzeige.printKlima(HOBBY, hobby);
  }
  
  if (keller.update()) {
    anzeige.printKlima(KELLER, keller);
  }

  // control actuators and supervise running actions
  if (kellerraum.update()) {
    anzeige.printModus(KELLER, kellerraum.modus);
  }

  if (hobbyraum.update()) {
    anzeige.printModus(HOBBY, hobbyraum.modus);
  }

  // actually control motor movement and display position
  Motor::move();

  int pos;

  if (kellerraum.moved(pos)) {
    anzeige.printFenster(KELLER, pos);
  }

  if (hobbyraum.moved(pos)) {
    anzeige.printFenster(HOBBY, pos);
  }

  // read touch screen
  Anzeige::Event evt = anzeige.getEvent();
  
  // execute user interaction
  switch (evt) {
  case Anzeige::EVT_NULL:
    break;

  case Anzeige::EVT_MODUS_KELLER:
    kellerraum.modus = kellerraum.modus == MANUELL ? AUTOMATISCH : MANUELL;
    anzeige.printModus(KELLER, kellerraum.modus);
    break;

  case Anzeige::EVT_MODUS_HOBBY:
    hobbyraum.modus = hobbyraum.modus == MANUELL ? AUTOMATISCH : MANUELL;
    anzeige.printModus(HOBBY, hobbyraum.modus);
    break;

  case Anzeige::EVT_AUF_KELLER:
    if (kellerraum.modus != ALARM) {
      kellerraum.modus = MANUELL;
      anzeige.printModus(KELLER, kellerraum.modus);
      kellerraum.fenster(AUF);
    }
    break;

  case Anzeige::EVT_AUF_HOBBY:
    if (hobbyraum.modus != ALARM) {
      hobbyraum.modus = MANUELL;
      anzeige.printModus(HOBBY, hobbyraum.modus);
      hobbyraum.fenster(AUF);
    }
    break;

  case Anzeige::EVT_ZU_KELLER:
    if (kellerraum.modus != ALARM) {
      kellerraum.modus = MANUELL;
      anzeige.printModus(KELLER, kellerraum.modus);
      kellerraum.fenster(ZU);
    }
    break;

  case Anzeige::EVT_ZU_HOBBY:
    if (hobbyraum.modus != ALARM) {
      hobbyraum.modus = MANUELL;
      anzeige.printModus(HOBBY, hobbyraum.modus);
      hobbyraum.fenster(ZU);
    }
    break;

  case Anzeige::EVT_STOP_KELLER:
    kellerraum.fenster(STOP);
    break;

  case Anzeige::EVT_STOP_HOBBY:
    hobbyraum.fenster(STOP);
    break;

  case Anzeige::EVT_CONFIG_KELLER:
    cfgOrt = KELLER;
    anzeige.screen(Anzeige::CONFIG, KELLER);
    anzeige.printCfg(s.getMinDiff(cfgOrt), s.getMaxDiff(cfgOrt), s.getMinTemp(cfgOrt));
    break;

  case Anzeige::EVT_CONFIG_HOBBY:
    cfgOrt = HOBBY;
    anzeige.screen(Anzeige::CONFIG, HOBBY);
    anzeige.printCfg(s.getMinDiff(cfgOrt), s.getMaxDiff(cfgOrt), s.getMinTemp(cfgOrt), s.getHeizWin());
    break;

  case Anzeige::EVT_CFG_HOME:
  {
    Settings::settings.save();
    anzeige.screen(Anzeige::HOME);
    anzeige.printKlima(AUSSEN, aussen);
    anzeige.printKlima(HOBBY, hobby);
    anzeige.printKlima(KELLER, keller);
    anzeige.printModus(KELLER, kellerraum.modus);
    anzeige.printModus(HOBBY, hobbyraum.modus);

    int pos;

    kellerraum.moved(pos);
    anzeige.printFenster(KELLER, pos);
    hobbyraum.moved(pos);
    anzeige.printFenster(HOBBY, pos);
  }
    break;

  case Anzeige::EVT_CFG_MIND_UP:
    s.setMinDiff(cfgOrt, s.getMinDiff(cfgOrt) + 0.1);
    anzeige.printCfg(s.getMinDiff(cfgOrt), NAN, NAN);
    break;

  case Anzeige::EVT_CFG_MIND_DN:
    s.setMinDiff(cfgOrt, s.getMinDiff(cfgOrt) - 0.1);
    anzeige.printCfg(s.getMinDiff(cfgOrt), NAN, NAN);
    break;

  case Anzeige::EVT_CFG_MAXD_UP:
    s.setMaxDiff(cfgOrt, s.getMaxDiff(cfgOrt) + 0.1);
    anzeige.printCfg(NAN, s.getMaxDiff(cfgOrt), NAN);
    break;

  case Anzeige::EVT_CFG_MAXD_DN:
    s.setMaxDiff(cfgOrt, s.getMaxDiff(cfgOrt) - 0.1);
    anzeige.printCfg(NAN, s.getMaxDiff(cfgOrt), NAN);
    break;

  case Anzeige::EVT_CFG_MINT_UP:
    s.setMinTemp(cfgOrt, s.getMinTemp(cfgOrt) + 0.1);
    anzeige.printCfg(NAN, NAN, s.getMinTemp(cfgOrt));
    break;

  case Anzeige::EVT_CFG_MINT_DN:
    s.setMinTemp(cfgOrt, s.getMinTemp(cfgOrt) - 0.1);
    anzeige.printCfg(NAN, NAN, s.getMinTemp(cfgOrt));
    break;

  case Anzeige::EVT_CFG_WIN_UP:
    s.setHeizWin(s.getHeizWin() + 1);
    anzeige.printCfg(NAN, NAN, NAN, s.getHeizWin());
    break;

  case Anzeige::EVT_CFG_WIN_DN:
    s.setHeizWin(s.getHeizWin() - 1);
    anzeige.printCfg(NAN, NAN, NAN, s.getHeizWin());
    break;
  }

  // send, receive and evaluate NTP packet
  time_t t = wlan.getNtpTime();

  if (t) {
    setTime(t);
    Log.print(hour());
    Log.print(":");
    Log.println(minute());
    Log.print(":");
    Log.println(second());
    anzeige.startlog();
  }

  // write log data to CSV file (every 5 minutes)
  anzeige.log(keller, hobby, aussen, kellerraum, hobbyraum);

  // handle web server requests
  wlan.serve(keller, hobby, aussen, kellerraum, hobbyraum);
}
