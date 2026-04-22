#include "ENS160_AQ.h"
#include <Wire.h>
#include "DFRobot_ENS160.h"

#define ENS160_SDA  6
#define ENS160_SCL  5
#define ENS160_ADDR 0x53

// Both ENS160 and BMV080 share Wire on GPIO6/5
// They have different I2C addresses so no conflict
static DFRobot_ENS160_I2C ens160(&Wire, ENS160_ADDR);
static bool ens160_ready = false;

void ENS160_Init(void)
{
  // Wire already initialised by BMV080_Init — no need to call begin() again
  Serial.print("ENS160: initialising... ");

  int attempts = 0;
  while (ens160.begin() != NO_ERR)
  {
    attempts++;
    Serial.printf("ENS160: FAILED (attempt %d)\n", attempts);
    if (attempts >= 3)
    {
      Serial.println("ENS160: giving up — check wiring");
      return;
    }
    delay(2000);
  }
  Serial.println("OK");

  ens160.setPWRMode(ENS160_STANDARD_MODE);

  Serial.println("ENS160: standard mode started");
  Serial.println("ENS160: status will show warming up for ~3 minutes");

  ens160_ready = true;
}

bool ENS160_IsReady(void)
{
  return ens160_ready;
}

bool ENS160_Read(ENS160_Record &outRecord)
{
  if (!ens160_ready) return false;

  outRecord.status       = ens160.getENS160Status();
  outRecord.eco2_ppm     = ens160.getECO2();
  outRecord.tvoc_ppb     = ens160.getTVOC();
  outRecord.aqi          = ens160.getAQI();
  outRecord.data_valid   = (outRecord.status == 0);
  outRecord.timestamp_ms = millis();

  return true;
}