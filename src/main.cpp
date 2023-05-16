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
  Serial.println("Starting smartDelay");
  unsigned long start = millis();
  do
  {
    while (ss.available())
    {
      Serial.println("Serial data available, encoding GPS data");
      gps.encode(ss.read());
    }
  } while (millis() - start < ms); // Loop until the specified delay time has passed
  Serial.println("smartDelay complete");
}

static void printFloat(float val, bool valid, int len, int prec)
{
  Serial.println("Starting printFloat");
  if (!valid) // Check if the value is valid
  {
    Serial.println("Invalid value, printing *");
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.println("Valid value, printing float");
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // Count the number of characters needed for the integer part, decimal point, and potential minus sign
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3
                         : vi >= 10    ? 2
                                       : 1; // Add the number of digits in the integer part
    for (int i = flen; i < len; ++i)
      Serial.print(' '); // Print spaces to fill the field width
  }
  smartDelay(0);
  Serial.println("printFloat complete");
}

static void printInt(unsigned long val, bool valid, int len)
{
  Serial.println("Starting printInt");
  char sz[32] = "*****************";
  if (valid) // Check if the value is valid
  {
    Serial.println("Valid value, printing integer");
    sprintf(sz, "%ld", val);
  }
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' '; // Print spaces to fill the field width
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
  Serial.println("printInt complete");
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  Serial.println("Starting printDateTime");
  if (!d.isValid()) // Check if the date is valid
  {
    Serial.println("Invalid date, printing *");
    Serial.print(F("********** "));
  }
  else
  {
    Serial.println("Valid date, printing date");
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid()) // Check if the time is valid
  {
    Serial.println("Invalid time, printing *");
    Serial.print(F("******** "));
  }
  else
  {
    Serial.println("Valid time, printing time");
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
  Serial.println("printDateTime complete");
}

static void printStr(const char *str, int len)
{
  Serial.println("Starting printStr");
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

  smartDelay(30000);
  Serial.println("Delay complete");

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS data received: check wiring"));
    Serial.println("GPS data check complete");
  }

  Serial.println("End of loop");
}
