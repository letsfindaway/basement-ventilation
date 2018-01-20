#include "anzeige.h"
#include "pins.h"
#include "logger.h"
#include "TimeLib.h"
#include "Timezone.h"

#include <Arduino.h>
#include <Fonts/FreeSansBold12pt7b.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 3870
#define TS_MAXX 170
#define TS_MINY 190
#define TS_MAXY 3840

#define HOT 25

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Bitmaps
 // 'fire'
static const unsigned char bmp_fire [] PROGMEM = {
  0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x07, 0xc0, 0x00, 
  0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x07, 0xf1, 0x00, 0x00, 0x0f, 0xf8, 0x80, 
  0x00, 0x0f, 0xf8, 0x80, 0x00, 0x1f, 0xfc, 0xc0, 0x02, 0x3f, 0xfd, 0xe0, 0x02, 0x3f, 0xfd, 0xf0, 
  0x06, 0x7f, 0xff, 0xf0, 0x06, 0xff, 0x7f, 0xf8, 0x0f, 0xff, 0x3f, 0xf8, 0x0f, 0xff, 0x1f, 0xf8, 
  0x1f, 0xff, 0x0f, 0xfc, 0x1f, 0xff, 0x0f, 0xfc, 0x1f, 0xff, 0x07, 0xfc, 0x3f, 0xf2, 0x07, 0xfc, 
  0x3f, 0xf2, 0x03, 0xfc, 0x3f, 0xe0, 0x03, 0xfc, 0x3f, 0xe0, 0x03, 0xf8, 0x3f, 0xc0, 0x03, 0xf8, 
  0x3f, 0x80, 0x03, 0xf8, 0x1f, 0x80, 0x03, 0xf0, 0x1f, 0x80, 0x03, 0xf0, 0x0f, 0x80, 0x03, 0xe0, 
  0x07, 0x80, 0x03, 0xc0, 0x03, 0x80, 0x03, 0x80, 0x00, 0x80, 0x02, 0x00, 0x00, 0x40, 0x04, 0x00
};
 // 'hand'
static const unsigned char bmp_hand [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 
  0x00, 0x60, 0xc0, 0x00, 0x00, 0x4e, 0x40, 0x00, 0x00, 0x4e, 0x40, 0x00, 0x00, 0x4e, 0x40, 0x00, 
  0x00, 0x4e, 0x40, 0x00, 0x00, 0x6e, 0x40, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 
  0x00, 0x0e, 0x00, 0x00, 0x00, 0x0e, 0xc0, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x0f, 0xfc, 0x00, 
  0x03, 0x8f, 0xfd, 0x80, 0x03, 0xcf, 0xfd, 0xc0, 0x03, 0xcf, 0xff, 0xc0, 0x01, 0xef, 0xff, 0xc0, 
  0x01, 0xff, 0xff, 0xc0, 0x00, 0xff, 0xff, 0xc0, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x7f, 0xff, 0xc0, 
  0x00, 0x7f, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x1f, 0xff, 0xc0, 
  0x00, 0x1f, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 // 'water-drop'
static const unsigned char bmp_water [] PROGMEM = {
  0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00, 
  0x00, 0x07, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x3f, 0xe0, 0x00, 
  0x00, 0x3f, 0xf0, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0xff, 0xfc, 0x00, 
  0x00, 0xff, 0xfe, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0x03, 0xff, 0xff, 0x80, 
  0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0, 0x07, 0x9f, 0xff, 0xe0, 
  0x07, 0x8f, 0xff, 0xe0, 0x07, 0x87, 0xff, 0xe0, 0x07, 0x83, 0xff, 0xe0, 0x07, 0x80, 0xff, 0xe0, 
  0x07, 0x80, 0x7f, 0xe0, 0x03, 0xc0, 0x7f, 0xc0, 0x03, 0xc0, 0x3f, 0xc0, 0x01, 0xf0, 0x7f, 0x80, 
  0x00, 0xf9, 0xff, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x07, 0xe0, 0x00
};
 // 'web-page-home'
static const unsigned char bmp_home [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x03, 0xe0, 0x00, 
  0x00, 0x07, 0xf0, 0x00, 0x00, 0x0e, 0x78, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x38, 0x1c, 0x00, 
  0x00, 0x70, 0x0e, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x01, 0xc0, 0x03, 0x80, 0x03, 0x80, 0x01, 0xc0, 
  0x07, 0x00, 0x00, 0xe0, 0x0e, 0x00, 0x00, 0x70, 0x1c, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x1c, 
  0x78, 0x00, 0x00, 0x1e, 0xfc, 0x00, 0x00, 0x3f, 0x7c, 0x00, 0x00, 0x7e, 0x0c, 0x0f, 0xf0, 0x70, 
  0x0c, 0x1f, 0xf0, 0x70, 0x0c, 0x1f, 0xf0, 0x70, 0x0c, 0x1c, 0x30, 0x70, 0x0c, 0x1c, 0x30, 0x70, 
  0x0c, 0x1c, 0x30, 0x70, 0x0c, 0x1c, 0x30, 0x70, 0x0c, 0x1c, 0x30, 0x70, 0x0c, 0x1c, 0x30, 0x70, 
  0x0f, 0xfc, 0x3f, 0xf0, 0x0f, 0xf8, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 // 'temp'
static const unsigned char bmp_temp [] PROGMEM = {
  0x00, 0x0f, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x39, 0xc0, 0x00, 0x00, 0x30, 0xcf, 0xc0, 
  0x00, 0x30, 0xcf, 0xc0, 0x00, 0x30, 0xc0, 0x00, 0x00, 0x30, 0xc0, 0x00, 0x00, 0x30, 0xcf, 0xc0, 
  0x00, 0x30, 0xcf, 0xc0, 0x00, 0x30, 0xc0, 0x00, 0x00, 0x30, 0xc0, 0x00, 0x00, 0x36, 0xcf, 0xc0, 
  0x00, 0x36, 0xcf, 0xc0, 0x00, 0x36, 0xc0, 0x00, 0x00, 0x36, 0xc0, 0x00, 0x00, 0x36, 0xc7, 0xc0, 
  0x00, 0x36, 0xc7, 0xc0, 0x00, 0x76, 0xe0, 0x00, 0x00, 0xf6, 0xf0, 0x00, 0x01, 0xcf, 0x38, 0x00, 
  0x01, 0xbf, 0xd8, 0x00, 0x03, 0xbf, 0xdc, 0x00, 0x03, 0x7f, 0xec, 0x00, 0x03, 0x7f, 0xec, 0x00, 
  0x03, 0x7f, 0xec, 0x00, 0x03, 0x7f, 0xec, 0x00, 0x03, 0xbf, 0xdc, 0x00, 0x01, 0xbf, 0xd8, 0x00, 
  0x01, 0xcf, 0x38, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x1f, 0x80, 0x00
};
 // 'window-closed'
static const unsigned char bmp_closed [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xc0, 0x03, 0xdf, 0xfb, 0xdf, 0xfb, 
  0xd4, 0x1b, 0xd0, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 
  0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 
  0xdf, 0xfb, 0xcf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 
  0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 
  0xdf, 0xfb, 0xdf, 0xfb, 0xc0, 0x03, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 // 'window-open'
static const unsigned char bmp_open [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0x01, 0x80, 0x01, 
  0xbf, 0xfd, 0xbf, 0xfd, 0xa0, 0x05, 0xa0, 0x0d, 0xab, 0xe5, 0xa0, 0x35, 0xa0, 0x05, 0xa0, 0x65, 
  0xa0, 0x05, 0xa0, 0xc5, 0xa0, 0x05, 0xa1, 0x95, 0xa0, 0x05, 0xa3, 0x35, 0xa0, 0x05, 0xa2, 0x65, 
  0xa0, 0x05, 0xa2, 0x85, 0xa0, 0x05, 0xa2, 0x05, 0xa0, 0x05, 0xa2, 0x05, 0xa0, 0x05, 0xa2, 0x05, 
  0xa0, 0x05, 0xa2, 0x05, 0xa0, 0x05, 0xa2, 0x05, 0xa0, 0x05, 0xa3, 0x05, 0xa0, 0x05, 0xa3, 0x85, 
  0xa0, 0x05, 0xa2, 0x05, 0xa0, 0x05, 0xa2, 0x05, 0xa0, 0x05, 0xa2, 0x05, 0xbf, 0xfd, 0xbe, 0x05, 
  0x80, 0x01, 0x82, 0x09, 0xff, 0xff, 0xfe, 0x1f, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x02, 0x40, 
  0x00, 0x00, 0x02, 0x80, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct Hotspot {
  int x;
  int y;
  Anzeige::Event event;
};

static const Hotspot hotspot_home [] PROGMEM = {
  { 75,  95, Anzeige::EVT_MODUS_KELLER},
  { 75, 215, Anzeige::EVT_MODUS_HOBBY},
  {225,  95, Anzeige::EVT_AUF_KELLER},
  {225, 215, Anzeige::EVT_AUF_HOBBY},
  {145,  95, Anzeige::EVT_ZU_KELLER},
  {145, 215, Anzeige::EVT_ZU_HOBBY},
  {185,  95, Anzeige::EVT_STOP_KELLER},
  {185, 215, Anzeige::EVT_STOP_HOBBY},
  {435,  95, Anzeige::EVT_CONFIG_KELLER},
  {435, 215, Anzeige::EVT_CONFIG_HOBBY},
  {  0,   0, Anzeige::EVT_NULL}
};

static const Hotspot hotspots_config [] PROGMEM = {
  {443,  46, Anzeige::EVT_CFG_HOME},
  {189,  90, Anzeige::EVT_CFG_MIND_DN},
  {369,  90, Anzeige::EVT_CFG_MIND_UP},
  {189, 140, Anzeige::EVT_CFG_MAXD_DN},
  {369, 140, Anzeige::EVT_CFG_MAXD_UP},
  {189, 190, Anzeige::EVT_CFG_MINT_DN},
  {369, 190, Anzeige::EVT_CFG_MINT_UP},
  {189, 240, Anzeige::EVT_CFG_WIN_DN},
  {369, 240, Anzeige::EVT_CFG_WIN_UP},
  {  0,   0, Anzeige::EVT_NULL}
};

Anzeige::Anzeige()
  : tft(TFT_CS, TFT_DC, -1), ts(STMPE_CS), lastevent(EVT_NULL), lasttime(0), nextminute(-1)
{

}

void Anzeige::setup() {
  if (!ts.begin()) {
    Serial.println("TS failed!");
  }

  tft.begin();
  tft.setRotation(1);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD failed!");
  }
}

void Anzeige::screen(Screen scr, Ort ort) {
  switch (scr) {
  case HOME:
    tft.fillScreen(HX8357_WHITE);
    bmpDraw("/Screen1.bmp", 0, 0);
    tft.setFont(&FreeSansBold12pt7b);
    tft.setTextColor(HX8357_BLACK);
    break;

  case CONFIG:
    tft.fillScreen(HX8357_WHITE);
    tft.fillRect(10, 10, 460, 300, 0xbdd7);
    tft.setCursor(22, 50);
    tft.print(ort == KELLER ? "Keller" : "Hobby");
    tft.drawBitmap(427, 30, bmp_home, 32, 32, HX8357_BLACK, 0xbdd7);

    tft.setCursor(22, 100);
    tft.print("Min. Diff.");
    tft.fillTriangle(180,83, 198,83, 189,98, HX8357_BLACK);
    tft.fillTriangle(360,98, 378,98, 369,83, HX8357_BLACK);

    tft.setCursor(22, 150);
    tft.print("Max. Diff.");
    tft.fillTriangle(180,133, 198,133, 189,148, HX8357_BLACK);
    tft.fillTriangle(360,148, 378,148, 369,133, HX8357_BLACK);

    tft.setCursor(22, 200);
    tft.print("Min. Temp.");
    tft.fillTriangle(180,183, 198,183, 189,198, HX8357_BLACK);
    tft.fillTriangle(360,198, 378,198, 369,183, HX8357_BLACK);

    if (ort == HOBBY) {
      tft.setCursor(22, 250);
      tft.print("Fenster %");
      tft.fillTriangle(180,233, 198,233, 189,248, HX8357_BLACK);
      tft.fillTriangle(360,248, 378,248, 369,233, HX8357_BLACK);
    }

    break;
  }

  current = scr;
}

void Anzeige::printKlima(Ort ort, Klima& klima) {
  if (current != HOME) {
    return;
  }

  tft.fillRect(140, 28+120*ort, 60, 21, 0xbdd7);
  tft.setCursor(140, 48+120*ort);
  tft.print(klima.temp, 1);
  
  tft.fillRect(250, 28+120*ort, 60, 21, 0xbdd7);
  tft.setCursor(250, 48+120*ort);
  tft.print(klima.humidity, 1);

  tft.fillRect(360, 28+120*ort, 60, 21, 0xbdd7);
  tft.setCursor(360, 48+120*ort);
  tft.print(klima.abshum, 1); 
}

void Anzeige::printModus(Ort ort, Modus modus)
{
  if (current != HOME) {
    return;
  }

  const unsigned char* bmp;
  uint16_t color;

  switch (modus) {
  case MANUELL:    bmp = bmp_hand;  color = tft.color565(0, 112, 30); break;
  case FEUCHT:     bmp = bmp_water; color = tft.color565(78, 0, 195); break;
  case TROCKEN:    bmp = bmp_water; color = tft.color565(0, 165, 44); break;
  case TEMPERATUR: bmp = bmp_temp;  color = tft.color565(78, 0, 195); break;
  case HEIZUNG:    bmp = bmp_fire;  color = tft.color565(195, 39, 0); break;
  default:         bmp = nullptr;
  }

  if (bmp) {
    tft.drawBitmap(75-16, 95-16+120*ort, bmp, 32, 32, color, 0xbdd7);
  } else {
    tft.fillRect(75-16, 95-16+120*ort, 32, 32, 0xbdd7);
  }
}


void Anzeige::printFenster(Ort ort, int pos)
{
  if (current != HOME) {
    return;
  }

  const unsigned char* bmp;
  uint16_t color;

  if (pos) {
    bmp = bmp_open;
    color = tft.color565(0, 150, 225);
  } else {
    bmp = bmp_closed;
    color = tft.color565(195, 46, 0);
  }

  tft.drawBitmap(185-16, 95-16+120*ort, bmp, 32, 32, color, 0xbdd7);

  int off = pos < 10 ? 20 : 0;

  tft.fillRect(270, 83+120*ort, 40, 21, 0xbdd7);
  tft.setCursor(270+off, 103+120*ort);
  tft.print(pos);
}

static bool within(int x, int y, const Hotspot& h) {
  return x > h.x - HOT
      && x < h.x + HOT
      && y > h.y - HOT
      && y < h.y + HOT;
}

void Anzeige::printCfg(float mind, float maxd, float mint, float win)
{
  if (current != CONFIG) {
    return;
  }

  if (!isnan(mind)) {
    tft.fillRect(240, 80, 60, 21, 0xbdd7);
    tft.setCursor(240, 100);
    tft.print(mind, 1);
  }

  if (!isnan(maxd)) {
    tft.fillRect(240, 130, 60, 21, 0xbdd7);
    tft.setCursor(240, 150);
    tft.print(maxd, 1);
  }

  if (!isnan(mint)) {
    tft.fillRect(240, 180, 60, 21, 0xbdd7);
    tft.setCursor(240, 200);
    tft.print(mint, 1);
  }

  if (!isnan(win)) {
    tft.fillRect(240, 230, 60, 21, 0xbdd7);
    tft.setCursor(240, 250);
    tft.print((int)win);
    tft.print("%");
  }
}

Anzeige::Event Anzeige::getEvent()
{
  if (lastevent != EVT_NULL) {
    if (ts.touched()) {
      // diesen Event nicht wiederholen bis losgelassen, alle Events droppen
      while (ts.bufferSize() > 1) {
        ts.getPoint();
     }

      return EVT_NULL;
    } else {
      lastevent = EVT_NULL;
    }
  }

  if (ts.bufferEmpty()) {
    return EVT_NULL;
  }

  // alle Events ausser dem letzten wegwerfen
  while (ts.bufferSize() > 1) {
    ts.getPoint();
  }

  TS_Point p = ts.getPoint();
  int y = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
  int x = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
//  Log.print("TS ");
//  Log.print(x);
//  Log.print("/");
//  Log.println(y);

  const Hotspot* hotspots = current == HOME ? hotspot_home : hotspots_config;

  for (size_t i = 0; hotspots[i].event != EVT_NULL; ++i) {
    if (within(x, y, hotspots[i])) {
      if ((long)millis() - lasttime < 200) {
        // gleicher Event innerhalb 200ms: ignorieren
        break;
      }
      
      lastevent = hotspots[i].event;
      lasttime = millis();
      Log.print("Detected event ");
      Log.println(lastevent);
      return lastevent;
    }
  }

  return EVT_NULL;
}

void Anzeige::log(Klima &k1, Klima &k2, Klima &k3, Raum &r1, Raum &r2)
{
#define LEN_ZEILE 79
  char zeile[LEN_ZEILE];
  tmElements_t tm;
  breakTime(now(), tm);

  if (tm.Minute != nextminute) {
    return;
  }

  char* p = zeile;
  p = fmt(p, tm.Year + 1970, 4);         *p++ = '-';
  p = fmt(p, tm.Month, 2);               *p++ = '-';
  p = fmt(p, tm.Day, 2);                 *p++ = ' ';
  p = fmt(p, tm.Hour, 2);                *p++ = ':';
  p = fmt(p, tm.Minute, 2);              *p++ = ';';

  p = fmt(p, 10 * k1.temp, 3, true);     *p++ = ';';
  p = fmt(p, 10 * k1.humidity, 3, true); *p++ = ';';
  p = fmt(p, 10 * k1.abshum, 3, true);   *p++ = ';';

  p = fmt(p, 10 * k2.temp, 3, true);     *p++ = ';';
  p = fmt(p, 10 * k2.humidity, 3, true); *p++ = ';';
  p = fmt(p, 10 * k2.abshum, 3, true);   *p++ = ';';

  p = fmt(p, 10 * k3.temp, 3, true,true);*p++ = ';';
  p = fmt(p, 10 * k3.humidity, 3, true); *p++ = ';';
  p = fmt(p, 10 * k3.abshum, 3, true);   *p++ = ';';

  p = fmt(p, r1.lastpos, 3);             *p++ = ';';
  p = fmt(p, r1.modus, 1);               *p++ = ';';

  p = fmt(p, r2.lastpos, 3);             *p++ = ';';
  p = fmt(p, r2.modus, 1);               *p++ = ';';
  p = fmt(p, digitalRead(ABGAS) == LOW ? 1 : 0, 1); *p++ = '\0';

  Log.println(zeile);
  nextminute = (nextminute + 5) % 60;

  // versuche Datei zu oeffnen
  File logfile;
  char filename[] = "yyyy/yyyymmdd";
  p = filename;
  p = fmt(p, tm.Year + 1970, 4) + 1;
  p = fmt(p, tm.Year + 1970, 4);
  p = fmt(p, tm.Month, 2);
  p = fmt(p, tm.Day, 2);

  // lege Verzeichnis an
  filename[4] = '\0';
  SD.mkdir(filename);
  filename[4] = '/';

  if ((logfile = SD.open(filename, FILE_WRITE | O_APPEND)) == 0) {
    Log.print(F("Kann Datei nicht oeffnen"));
    return;
  }

  logfile.println(zeile);
  logfile.close();
}


// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.
#define BUFFPIXEL 80

void Anzeige::bmpDraw(const char *filename, uint8_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == 0) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}


char* Anzeige::fmt(char *s, float nbr, int digits, bool decimal, bool sign)
{
  int n = nbr + 0.5;
  char* p = s + digits + (decimal ? 1 : 0) + (sign ? 1 : 0);
  char* r = p;
  *--p = '0' + n % 10;
  n /= 10;

  if (decimal) {
    *--p = ',';
  }

  while (--digits) {
    *--p = '0' + n % 10;
    n /= 10;
  }

  if (sign) {
    *--p = nbr < 0 ? '-' : ' ';
  }

  return r;
}


void Anzeige::startlog()
{
  nextminute = (5 * ((minute() + 5) / 5)) % 60;
}
