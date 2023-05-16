#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
/*
   This sample code demonstrates the normal use of a TinyGPSPlus (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3
                         : vi >= 10    ? 2
                                       : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
  smartDelay(0);
}

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);

  Serial.println(F("FullExample.ino"));
  Serial.println(F("An extensive example of many interesting TinyGPSPlus features"));
  Serial.print(F("Testing TinyGPSPlus library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to Brasilia  ----  RX    RX        Fail"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
}

void loop()
{
  Serial.println("Starting loop");
  
  static const double BRASILIA_LAT = -15.73905, BRASILIA_LON = -47.89370;
  Serial.println("Brasilia coordinates set");

  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  Serial.println("Satellite value printed");
  
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  Serial.println("HDOP value printed");

  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  Serial.println("Latitude value printed");
  
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  Serial.println("Longitude value printed");

  printInt(gps.location.age(), gps.location.isValid(), 5);
  Serial.println("Location age printed");

  printDateTime(gps.date, gps.time);
  Serial.println("Date and time printed");
  
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  Serial.println("Altitude printed");
  
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  Serial.println("Course degrees printed");

  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  Serial.println("Speed printed");

  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);
  Serial.println("Course direction printed");

  unsigned long distanceKmToBrasilia =
      (unsigned long)TinyGPSPlus::distanceBetween(
          gps.location.lat(),
          gps.location.lng(),
          BRASILIA_LAT,
          BRASILIA_LON) /
      1000;
  printInt(distanceKmToBrasilia, gps.location.isValid(), 9);
  Serial.println("Distance to Brasilia printed");

  double courseToBrasilia =
      TinyGPSPlus::courseTo(
          gps.location.lat(),
          gps.location.lng(),
          BRASILIA_LAT,
          BRASILIA_LON);
  printFloat(courseToBrasilia, gps.location.isValid(), 7, 2);
  Serial.println("Course to Brasilia printed");

  const char *cardinalToBrasilia = TinyGPSPlus::cardinal(courseToBrasilia);
  printStr(gps.location.isValid() ? cardinalToBrasilia : "*** ", 6);
  Serial.println("Cardinal direction to Brasilia printed");

  printInt(gps.charsProcessed(), true, 6);
  Serial.println("Characters processed printed");

  printInt(gps.sentencesWithFix(), true, 10);
  Serial.println("Sentences with fix printed");

  printInt(gps.failedChecksum(), true, 9);
  Serial.println("Failed checksums printed");

  Serial.println();
  Serial.println("Printed all values, about to delay");

  smartDelay(1000);
  Serial.println("Delay complete");

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS data received: check wiring"));
    Serial.println("GPS data check complete");
  }
  
  Serial.println("End of loop");
}
