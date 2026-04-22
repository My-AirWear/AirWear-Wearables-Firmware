#include "GPS_Module.h"
#include <TinyGPSPlus.h>

static const int GPS_RX_PIN = 8;   // ESP32 receives from GPS TX
static const int GPS_TX_PIN = 7;   // ESP32 transmits to GPS RX

static HardwareSerial GPSSerial(2);
static TinyGPSPlus gps;

int GPS_Available(void)
{
  return GPSSerial.available();
}

void GPS_Init(void)
{
  GPSSerial.begin(115200, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  delay(200);
  Serial.println("GPS serial started");
}

void GPS_Update(void)
{
  while (GPSSerial.available() > 0)
  {
    gps.encode(GPSSerial.read());
  }
}

bool GPS_Get_Record(GPS_Record &outRecord)
{
  outRecord.valid_location = gps.location.isValid();
  outRecord.valid_date = gps.date.isValid();
  outRecord.valid_time = gps.time.isValid();

  outRecord.latitude = gps.location.isValid() ? gps.location.lat() : 0.0;
  outRecord.longitude = gps.location.isValid() ? gps.location.lng() : 0.0;
  outRecord.altitude_m = gps.altitude.isValid() ? gps.altitude.meters() : 0.0;
  outRecord.speed_kmph = gps.speed.isValid() ? gps.speed.kmph() : 0.0;
  outRecord.satellites = gps.satellites.isValid() ? gps.satellites.value() : 0;

  outRecord.year = gps.date.isValid() ? gps.date.year() : 0;
  outRecord.month = gps.date.isValid() ? gps.date.month() : 0;
  outRecord.day = gps.date.isValid() ? gps.date.day() : 0;

  outRecord.hour = gps.time.isValid() ? gps.time.hour() : 0;
  outRecord.minute = gps.time.isValid() ? gps.time.minute() : 0;
  outRecord.second = gps.time.isValid() ? gps.time.second() : 0;

  outRecord.age_ms = gps.location.isValid() ? gps.location.age() : 0xFFFFFFFF;

  return gps.location.isValid() || gps.date.isValid() || gps.time.isValid();
}
