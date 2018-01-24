/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#include "pins.h"
#include "klima.h"
#include "anzeige.h"
#include "raum.h"
#include "motor.h"
#include "settings.h"
#include "wlan.h"
#include "logger.h"

#include "Time.h"

Klima aussen(AUSSEN, DHT22_AUSSEN);
Klima keller(KELLER, DHT22_KELLER);
Klima hobby (HOBBY, DHT22_HOBBY);
Anzeige anzeige;
Raum kellerraum(KELLER, keller, aussen);
Raum hobbyraum(HOBBY, hobby, aussen);

Wlan wlan;

extern "C" char *sbrk(int i);

int FreiRam () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

void setup() {
  // Pins konfigurieren
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

  Serial.begin(9600);
  int cnt = 50;
  while (!Serial && cnt--) delay(100);
  delay(500);
  Serial.println("Fenstersteuerung");
  Serial.print("Freier Speicher: ");
  Serial.println(FreiRam());

  analogReadResolution(10);

  Serial.println(F("Anzeige initialisieren"));
  anzeige.setup();
  anzeige.screen(Anzeige::HOME);

  Serial.println(F("Wlan initialisieren"));
  wlan.connect();

  // Jetzt kann der Logger benutzt werden
  Log.begin();
  Log.println("Start UDP Logger");
  Log.println(F("Sensoren initialisieren"));
  Settings::settings.load();

  aussen.setup(0, 30);
  keller.setup(10, 30);
  hobby.setup(20, 30);

  Log.print("Freier Speicher: ");
  Log.println(FreiRam());

  Log.println("Motoren kalibrieren");
  kellerraum.setup();
  hobbyraum.setup();
  Settings::settings.save();

  Log.println("Fenster schliessen");
  kellerraum.fenster(ZU);
  hobbyraum.fenster(ZU);
  Log.println("Setup beendet");

  wlan.getNtpTime();
}

void loop() {
  static Ort cfgOrt;
  Settings& s(Settings::settings);

//  delay(10);

  // Sensoren einlesen, Klima nur alle 30 sec oder so
  if (aussen.update()) {
    anzeige.printKlima(AUSSEN, aussen);
  }

  if (hobby.update()) {
    anzeige.printKlima(HOBBY, hobby);
  }
  
  if (keller.update()) {
    anzeige.printKlima(KELLER, keller);
  }

  // Aktoren ansteuern und laufende Aktionen ueberwachen
  if (kellerraum.update()) {
    anzeige.printModus(KELLER, kellerraum.modus);
  }

  if (hobbyraum.update()) {
    anzeige.printModus(HOBBY, hobbyraum.modus);
  }

  Motor::move();

  int pos;

  if (kellerraum.moved(pos)) {
    anzeige.printFenster(KELLER, pos);
  }

  if (hobbyraum.moved(pos)) {
    anzeige.printFenster(HOBBY, pos);
  }

  // Touch auslesen
  Anzeige::Event evt = anzeige.getEvent();
  
  // Benutzerinteraktion ausfuehren
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
    kellerraum.modus = MANUELL;
    anzeige.printModus(KELLER, kellerraum.modus);
    kellerraum.fenster(AUF);
    break;

  case Anzeige::EVT_AUF_HOBBY:
    hobbyraum.modus = MANUELL;
    anzeige.printModus(HOBBY, hobbyraum.modus);
    hobbyraum.fenster(AUF);
    break;

  case Anzeige::EVT_ZU_KELLER:
    kellerraum.modus = MANUELL;
    anzeige.printModus(KELLER, kellerraum.modus);
    kellerraum.fenster(ZU);
    break;

  case Anzeige::EVT_ZU_HOBBY:
    hobbyraum.modus = MANUELL;
    anzeige.printModus(HOBBY, hobbyraum.modus);
    hobbyraum.fenster(ZU);
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

  // NTP Paket senden/empfangen und auswerten (alle 15 Minuten)
  time_t t = wlan.getNtpTime();

  if (t) {
    setTime(t);
    Log.print(hour());
    Log.print(":");
    Log.println(minute());
    anzeige.startlog();
  }

  // Log-Daten aufzeichnen (alle 5 Minuten)
  anzeige.log(keller, hobby, aussen, kellerraum, hobbyraum);
  wlan.serve(keller, hobby, aussen, kellerraum, hobbyraum);
}
