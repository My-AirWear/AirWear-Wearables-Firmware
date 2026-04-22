// ============================================================
//  BMV080 Step 1 — Sensor Test Sketch
//  Board  : Waveshare ESP32-S3-Touch-LCD-1.47
//  Sensor : DFRobot Fermion BMV080 (SEN0663)
//  Comms  : I2C — SDA = GPIO6, SCL = GPIO5
//  Address: 0x57 (factory default, confirmed by scanner)
// ============================================================

#include <Wire.h>
#include "DFRobot_BMV080.h"

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
  Serial.println("  BMV080 PM Sensor — Step 1 Test");
  Serial.println("========================================\n");

  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.print("Initialising BMV080... ");
  while (sensor.begin() != 0) {
    Serial.println("FAILED — check wiring and I2C address");
    Serial.println("Retrying in 3 seconds...");
    delay(3000);
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

  Serial.print("Opening sensor handle... ");
  uint16_t openResult = sensor.openBmv080();
  Serial.print("openBmv080 result code: ");
  Serial.println(openResult);
  if (openResult != 0) {
    Serial.println("FAILED");
    while (true) delay(1000);
  }
  Serial.println("OK");

  Serial.print("Starting continuous measurement... ");
  if (sensor.setBmv080Mode(CONTINUOUS_MODE) != 0) {
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
      Serial.print(" OBSTRUCTED — clear 35cm in front of sensor");
    } else {
      Serial.print(" OK");
    }
    Serial.println();
  }
  delay(500);
}