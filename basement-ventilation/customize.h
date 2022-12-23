/*
 * Copyright (c) letsfindaway. All rights reserved.
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */

#ifndef CUSTOMIZE_H
#define CUSTOMIZE_H

// Definition of pins

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

// Current limit values (1 Ohm, ARef=3,3V, 10 bits)

#define BV_CURRENT(mA)        int((mA)*1024/3300)
#define BV_MILLIAMP(dac)      int((dac)*3300/1024)
#define BV_CURRENT_LOW        BV_CURRENT(30)
#define BV_CURRENT_HIGH       BV_CURRENT(500)
#define BV_MOTOR_STARTUP_TIME 200
#define BV_MOTOR_RAMP_TIME    2000

// Name of rooms and labels

#define BV_NAME_ROOM1         "Keller"
#define BV_NAME_ROOM2         "Hobby"
#define BV_TEXT_MINDIFF       "Min. Diff."
#define BV_TEXT_MAXDIFF       "Max. Diff."
#define BV_TEXT_MINTEMP       "Min. Temp."
#define BV_TEXT_WINDOWPERCENT "Fenster %"

// Mode characters used to indicate mode in SVG display
// M = manual
// A = automatic
// F = feucht (humid)
// T = trocken (dry)
// H = Heizung (heating)
// K = kalt (cold)
// ! = Alarm
#define BV_SVG_MODECHARS      "MAFTHK!"

// Decimal separator for CSV

#define BV_CSV_DECIMAL        ','

// Network addresses and ports
// use commas for IP addresses, as they are used as parameter for IPAddress

#define BV_NTP_SERVER 192,168,178,1
#define BV_LOG_TARGET 192,168,178,20
#define BV_LOG_PORT   4242

// SPI bus holdoff time (see doc/spi.md)

#define SPI_HOLDOFF_TIMEMS 10

#endif
