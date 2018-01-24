/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#ifndef PINS_H
#define PINS_H

// Definition der Pins

#define DHT22_AUSSEN 11
#define DHT22_KELLER 12
#define DHT22_HOBBY  19

#define MOTOR_AN      13
#define ABGAS         14
#define FENSTER_AUF   15
#define FENSTER_ZU    16
#define FENSTER_HOBBY 17
#define MOTOR_STROM   18

#define RELAY_ON  LOW
#define RELAY_OFF HIGH

#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5

enum Ort {
  KELLER,
  HOBBY,
  AUSSEN
};

#endif
