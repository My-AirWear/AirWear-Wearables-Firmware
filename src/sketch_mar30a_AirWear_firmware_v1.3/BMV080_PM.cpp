#include "BMV080_PM.h"
#include <Wire.h>
#include "DFRobot_BMV080.h"

// BMV080 on its own I2C bus
// SDA = GPIO6, SCL = GPIO5
#define BMV080_SDA 6
#define BMV080_SCL 5
#define BMV080_ADDR 0x57

static DFRobot_BMV080_I2C bmv080(&Wire, BMV080_ADDR);
static bool bmv080_ready = false;

void BMV080_Init(void)
{
  // Wire is shared with nothing else on this bus
  Wire.begin(BMV080_SDA, BMV080_SCL);

  Serial.println("BMV080: waiting 5s for sensor startup...");
  delay(5000);

  int attempts = 0;
  while (bmv080.begin() != 0)
  {
    attempts++;
    Serial.printf("BMV080: begin() failed (attempt %d), retrying...\n", attempts);
    delay(2000);
    if (attempts >= 10)
    {
      Serial.println("BMV080: could not initialise after 10 attempts");
      return;
    }
  }
  Serial.println("BMV080: begin() OK");

  // Delay after begin() before opening handle —
  // required by DFRobot startup timing guidance
  Serial.println("BMV080: waiting 3s before opening handle...");
  delay(3000);

  uint16_t openResult = bmv080.openBmv080();
  if (openResult != 0)
  {
    Serial.printf("BMV080: openBmv080() failed, code: %u\n", openResult);
    return;
  }
  Serial.println("BMV080: handle opened OK");

  // Delay after opening handle before setting mode
  Serial.println("BMV080: waiting 2s before setting mode...");
  delay(2000);

  if (bmv080.setBmv080Mode(CONTINUOUS_MODE) != 0)
  {
    Serial.println("BMV080: setBmv080Mode() failed");
    return;
  }
  Serial.println("BMV080: continuous mode started");

  bmv080_ready = true;
}

bool BMV080_IsReady(void)
{
  return bmv080_ready;
}

bool BMV080_Read(BMV080_Record &outRecord)
{
  if (!bmv080_ready) return false;

  float pm1, pm2_5, pm10;

  // getBmv080Data() returns 1 when fresh data is ready,
  // 0 when the sensor is still processing internally
  if (bmv080.getBmv080Data(&pm1, &pm2_5, &pm10) != 1)
    return false;

  outRecord.pm1_0        = pm1;
  outRecord.pm2_5        = pm2_5;
  outRecord.pm10         = pm10;
  outRecord.obstructed   = bmv080.ifObstructed();
  outRecord.timestamp_ms = millis();

  return true;
}