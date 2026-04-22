#ifndef BMV080_PM_H
#define BMV080_PM_H

#include <Arduino.h>

struct BMV080_Record
{
  float pm1_0;
  float pm2_5;
  float pm10;
  bool  obstructed;
  uint32_t timestamp_ms;
};

void BMV080_Init(void);
bool BMV080_Read(BMV080_Record &outRecord);
bool BMV080_IsReady(void);

#endif