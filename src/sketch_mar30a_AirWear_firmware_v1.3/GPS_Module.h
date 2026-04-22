#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>

struct GPS_Record
{
  bool valid_location;
  bool valid_date;
  bool valid_time;

  double latitude;
  double longitude;
  double altitude_m;
  double speed_kmph;
  uint32_t satellites;

  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  uint32_t age_ms;
};

void GPS_Init(void);
void GPS_Update(void);
bool GPS_Get_Record(GPS_Record &outRecord);
int GPS_Available(void);   //raw UART availability checks to see if the GPS is working

#endif
