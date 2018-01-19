#include "wlan.h"
#include "logger.h"

#include <SPI.h>
#include <WiFiUdp.h>

#include "secret/credentials.h"

int status = WL_IDLE_STATUS;     // the WiFi radio's status

unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(192, 168, 178, 1); // fritz.box
//IPAddress timeServer(85, 10, 240, 253); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
// A TCP server
WiFiServer server(80);
WiFiClient client;


Wlan::Wlan()
  : lastRequest(0), phase(IDLE), currentLineIsBlank(false)
{
}

bool Wlan::connect()
{
  WiFi.setPins(8,7,4,2);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    return false;
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  bool ok =  WiFi.status() == WL_CONNECTED;

  Serial.println(ok ? "You're connected to the network" : "You're not connected to the network");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  delay(2000);
  Serial.print("Ping ");
  Serial.println(WiFi.ping(timeServer));
  Udp.begin(localPort);
  Serial.println("Starte Server");
  server.begin();
  return ok;
}


time_t Wlan::getNtpTime()
{
  if (lastRequest == 0 || millis() - lastRequest > 15 * 60000) {
    lastRequest = millis();
    sendNTPpacket(timeServer);
  }

  int size = Udp.parsePacket();

  if (size >= NTP_PACKET_SIZE) {
    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
    Log.println("Empfange NTP Antwort");
    Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
    unsigned long secsSince1900;
    // convert four bytes starting at location 40 to a long integer
    secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
    secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
    secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
    secsSince1900 |= (unsigned long)packetBuffer[43];
    return secsSince1900 - 2208988800UL;
  }

  return 0; // return 0 if unable to get the time
}


void Wlan::serve(Klima &k1, Klima &k2, Klima &k3, Raum &r1, Raum &r2) {

  switch (phase) {
  case IDLE:
    client = server.available();

    if (client) {
      Log.println("Verbindung!");
      if (client.connected()) {
        Log.println("connected");
      }
      // Anfrage ist reingekommen! Wir lesen die GET Zeile der Form GET /yyyymm[dd][.csv] blabla
      char buf[10];
      size_t bytes = client.readBytesUntil('/', buf, 10);

      if (bytes == 10) {
        client.stop();
        return;
      }

      req = client.readStringUntil(' ');
      Log.print("Anfrage fuer ");
      Log.println(req);
      currentLineIsBlank = false;
      phase = SKIP_HEADER;
    }

    break;

  case SKIP_HEADER:
    Log.println("Skip header");
    while (client.available()) {
      char c = client.read();
      if (c == '\n' && currentLineIsBlank) {
        if (req.length() == 0) {
          phase = RESPONDSVG;
          break;
        } else if (req.length() < 6) {
          Log.println("Dateiname zu kurz");
          client.stop();
          phase = IDLE;
          return;
        }

        // analysiere den req String
        if (req.endsWith(".csv")) {
          req = req.substring(0, req.length() - 4);
        }

        if (req.length() == 8) {
          tag = 0;
          phase = RESPOND;
        } else if (req.length() == 6) {
          tag = 1;
          phase = RESPOND;
        } else {
          // schreibe 404 Antwort
          client.println("HTTP/1.1 404 Not Found");
          client.println();
          delay(1);
          client.stop();
          phase = IDLE;
        }

        break;
      }

      if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
      }
      else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
      }
    }

    break;

  case RESPOND:
    // erzeuge Filename aus dem req String
    // req = yyyymmdd.csv -> filename = yyyy/yyyymmdd
    req = req.substring(0, 4) + "/" + req;

    if (tag == 0) {
      // wenn nur ein Tag angefordert: gibt es diesen File?
      daten = SD.open(req);

      if (!daten) {
        // schreibe 404 Antwort
        client.println("HTTP/1.1 404 Not Found");
        client.println();
        delay(1);
        client.stop();
        phase = IDLE;
        break;
      }
    }

    // schreibe 200 Antwort
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/csv");
    client.println("Connection: close");  // the connection will be closed after completion of the response
    client.println();

    // schreibe den CSV Header
    client.println("Zeit;T Keller;H Keller;A Keller;T Hobby;H Hobby;A Hobby;T Aussen;H Aussen;A Aussen;Pos Keller;Mode Keller;Pos Hobby;Mode Hobby;Heizung");
    phase = SEND_DATA;
    break;

  case RESPONDSVG:
    // schreibe 200 Antwort
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/xhtml+xml");
    client.println("Connection: close");  // the connection will be closed after completion of the response
    client.println();
    sendSVG(k1, k2, k3, r1, r2);
    delay(1);
    client.stop();
    phase = IDLE;
    break;

  case SEND_DATA:
    if (!daten) {
      if (tag == 0 || tag > 31) {
        // Monatsabfrage beendet
        client.stop();
        phase = IDLE;
        break;
      }

      // Monatsanfrage und naechster File erforderlich
      String filename = req + (char)((tag / 10) + '0') + (char)((tag % 10) + '0');
      ++tag;
      daten = SD.open(filename);

      if (!daten) {
        // File existiert nicht, versuche den naechsten
        break;
      }
    }

    for (int i = 0; i < 8; ++i) {
      String line = daten.readStringUntil('\n');

      if (line.endsWith("\r")) {
        line = line.substring(0, line.length() - 1);
      }

      if (line.length() == 0) {
        daten.close();

        if (tag == 0) {
          // nur ein Tag angefordert, beende Verbindung
          client.stop();
          phase = IDLE;
        }

        break;
      }

      client.write(line.c_str(), line.length());
      client.write('\n');
    }

    break;
  }
}


// send an NTP request to the time server at the given address
void Wlan::sendNTPpacket(IPAddress& address)
{
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Log.print("Sende NTP Paket ");
  Log.println(Udp.endPacket());
  //Serial.println("6");
}


void Wlan::sendSVG(Klima &k1, Klima &k2, Klima &k3, Raum &r1, Raum &r2)
{
  /*
<!DOCTYPE html>
<html>
<body>

<h1>My first SVG</h1>

<svg width="480" height="320" style="font-family:Arial;font-size:24px;font-weight:bold"  transform="scale(1)">
   <rect x="10" y="10" width="460" height="110" style="fill:rgb(200,200,200);stroke-width:0" />
   <rect x="10" y="130" width="460" height="110" style="fill:rgb(200,200,200);stroke-width:0" />
   <rect x="10" y="250" width="460" height="60" style="fill:rgb(200,200,200);stroke-width:0" />
   <text x="20" y="50">Keller</text>
   <text x="140" y="50">18.5 &deg;C</text>
   <text x="250" y="50">37.5 %</text>
   <text x="360" y="50">5.3 g/m&sup3;</text>
   <text x="40" y="105" style="fill:rgb(0,0,200)">M</text>
   <text x="250" y="105">100 %</text>

   <text x="20" y="170">Hobby</text>
   <text x="140" y="170">18.5 &deg;C</text>
   <text x="250" y="170">37.5 %</text>
   <text x="360" y="170">5.3 g/m&sup3;</text>
   <text x="40" y="225" style="fill:rgb(0,0,200)">M</text>
   <text x="250" y="225">100 %</text>

   <text x="20" y="290">Au&szlig;en</text>
   <text x="140" y="290">18.5 &deg;C</text>
   <text x="250" y="290">37.5 %</text>
   <text x="360" y="290">5.3 g/m&sup3;</text>
   Sorry, your browser does not support inline SVG.
</svg>

</body>
</html>
   */
  client.println("<!DOCTYPE html>");
  client.println("<html><body>");
  client.print("<svg width=\"480\" height=\"320\" style=\"font-family:Arial;font-size:24px;font-weight:bold\">");
  client.print("<rect x=\"10\" y=\"10\" width=\"460\" height=\"110\" style=\"fill:rgb(200,200,200);stroke-width:0\" />");
  client.print("<rect x=\"10\" y=\"130\" width=\"460\" height=\"110\" style=\"fill:rgb(200,200,200);stroke-width:0\" />");
  client.print("<rect x=\"10\" y=\"250\" width=\"460\" height=\"60\" style=\"fill:rgb(200,200,200);stroke-width:0\" />");
  client.print("<text x=\"20\" y=\"50\">Keller</text>");
  client.print("<text x=\"20\" y=\"170\">Hobby</text>");
  client.print("<text x=\"20\" y=\"290\">Au&szlig;en</text>");
  sendSVGKlima(k1, 50);
  sendSVGKlima(k2, 170);
  sendSVGKlima(k3, 290);
  sendSVGRaum(r1, 105);
  sendSVGRaum(r2, 225);
  client.println("</svg>");
  client.println("</body></html>");
}


void Wlan::sendSVGKlima(Klima &k, int y)
{
  client.print("<text x=\"140\" y=\"");
  client.print(y);
  client.print("\">");
  client.print(k.temp, 1);
  client.print(" &deg;C</text>");

  client.print("<text x=\"250\" y=\"");
  client.print(y);
  client.print("\">");
  client.print(k.humidity, 1);
  client.print(" %</text>");

  client.print("<text x=\"360\" y=\"");
  client.print(y);
  client.print("\">");
  client.print(k.abshum, 1);
  client.print(" g/m&sup3;</text>");
}


void Wlan::sendSVGRaum(Raum &r, int y)
{
  const static char* modechar[] = { "M", "A", "F", "T", "H", "K" };
  client.print("<text x=\"40\" y=\"");
  client.print(y);
  client.print("\">");
  client.print(modechar[r.modus]);
  client.print("</text>");

  client.print("<text x=\"250\" y=\"");
  client.print(y);
  client.print("\">");
  client.print(r.lastpos);
  client.print(" %</text>");
}