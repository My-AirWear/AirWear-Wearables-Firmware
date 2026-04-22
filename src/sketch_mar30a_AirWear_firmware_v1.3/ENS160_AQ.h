#ifndef ENS160_AQ_H
#define ENS160_AQ_H

#include <Arduino.h>

struct ENS160_Record
{
  uint16_t eco2_ppm;    // equivalent CO2 in ppm (parts per million)
  uint16_t tvoc_ppb;    // total VOCs in ppb (parts per billion)
  uint8_t  aqi;         // air quality index 1-5 (1=excellent, 5=unhealthy)
  uint8_t  status;      // 0=OK, 1=warming up, 2=initial start, 3=invalid
  bool     data_valid;  // true when status is 0 (normal operation)
  uint32_t timestamp_ms;
};

void ENS160_Init(void);
bool ENS160_Read(ENS160_Record &outRecord);
bool ENS160_IsReady(void);

#endif