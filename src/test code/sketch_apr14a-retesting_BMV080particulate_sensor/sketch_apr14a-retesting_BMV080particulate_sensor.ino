#include <Wire.h>
#include "DFRobot_BMV080.h"
/*this sensor needs startup delays to start working: 5 seconds before begin()
3 seconds before openBmv080()
2 seconds before setBmv080Mode()
Total: about 10 seconds of startup time to budget for in your boot sequence */

#define I2C_SDA 6
#define I2C_SCL 5
#define BMV080_ADDR 0x57

SET_LOOP_TASK_STACK_SIZE(60 * 1024);

DFRobot_BMV080_I2C sensor(&Wire, BMV080_ADDR);

float pm1   = 0.0;
float pm2_5 = 0.0;
float pm10  = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("========================================");
  Serial.println("  BMV080 Test — with startup delay");
  Serial.println("========================================\n");

  Wire.begin(I2C_SDA, I2C_SCL);

  // Wait for sensor to complete hardware init,
  // laser preheat and optical self-test as per
  // DFRobot's startup timing requirement
  Serial.println("Waiting 5 seconds for sensor startup...");
  delay(5000);

  Serial.print("Initialising BMV080... ");
  int attempts = 0;
  while (sensor.begin() != 0) {
    attempts++;
    Serial.printf("FAILED (attempt %d) — retrying in 2s\n", attempts);
    delay(2000);
    if (attempts >= 10) {
      Serial.println("Could not initialise sensor after 10 attempts. Check wiring.");
      while (true) delay(1000);
    }
  }
  Serial.println("OK");

  char chipID[13];
  if (sensor.getBmv080ID(chipID)) {
    Serial.print("Chip ID    : ");
    Serial.println(chipID);
  }

  uint16_t major, minor, patch;
  if (sensor.getBmv080DV(major, minor, patch)) {
    Serial.print("SDK version: ");
    Serial.print(major); Serial.print(".");
    Serial.print(minor); Serial.print(".");
    Serial.println(patch);
  }

  // Additional delay after begin() before opening handle
  // as per DFRobot startup timing guidance
  Serial.println("Waiting 3 more seconds before opening handle...");
  delay(3000);

Serial.print("Opening sensor handle... ");
  uint16_t openResult = sensor.openBmv080();
  Serial.printf("result code: %u\n", openResult);
  if (openResult != 0) {
    Serial.println("FAILED — openBmv080 returned error");
    while (true) delay(1000);
  }
  Serial.println("OK");

  // Additional delay after opening handle before setting mode
  Serial.println("Waiting 2 more seconds before setting mode...");
  delay(2000);

  Serial.print("Starting continuous measurement... ");
  int modeResult = sensor.setBmv080Mode(CONTINUOUS_MODE);
  Serial.printf("mode result code: %d\n", modeResult);
  if (modeResult != 0) {
    Serial.println("FAILED");
    while (true) delay(1000);
  }
  Serial.println("OK\n");

  Serial.println("Sensor warming up — readings will appear shortly...");
  Serial.println("----------------------------------------");
  Serial.println("  PM1      PM2.5    PM10    Status");
  Serial.println("  ug/m3    ug/m3    ug/m3");
  Serial.println("----------------------------------------");
}

void loop() {
  if (sensor.getBmv080Data(&pm1, &pm2_5, &pm10) == 1) {
    Serial.printf("  %-8.1f %-8.1f %-8.1f", pm1, pm2_5, pm10);
    if (sensor.ifObstructed()) {
      Serial.print(" OBSTRUCTED");
    } else {
      Serial.print(" OK");
    }
    Serial.println();
  }
  delay(500);
}