// ============================================================
//  ENS160 Step 1 — Sensor Test Sketch
//  Board  : Waveshare ESP32-S3-Touch-LCD-1.47
//  Sensor : DFRobot Fermion ENS160 (SEN0515)
//  Comms  : I2C — SDA = GPIO6, SCL = GPIO5
//  Address: 0x53 (default)
// ============================================================
//
//  WHAT THIS SKETCH DOES:
//  1. Initialises the ENS160 over I2C
//  2. Waits 3 minutes for warm-up (sensor requirement)
//  3. Reads eCO2, TVOC and AQI every 2 seconds
//  4. Prints readable output to Serial Monitor
//
//  NOTE: eCO2 and TVOC readings are unreliable for the first
//  3 minutes after power-on — this is normal, the sensor
//  needs to heat its MOX element to operating temperature.
//  A status field tells you when it's ready.
// ============================================================

#include <Wire.h>
#include "DFRobot_ENS160.h"

#define I2C_SDA 6
#define I2C_SCL 5

// I2C address — 0x53 is default on DFRobot breakout
// (can be changed to 0x52 by soldering the address pad)
DFRobot_ENS160_I2C ens160(&Wire, 0x53);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("========================================");
  Serial.println("  ENS160 Air Quality Sensor Test");
  Serial.println("========================================\n");

  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.print("Initialising ENS160... ");
  while (ens160.begin() != NO_ERR) {
    Serial.println("FAILED — check wiring and I2C address");
    Serial.println("Retrying in 3 seconds...");
    delay(3000);
  }
  Serial.println("OK\n");

  // Set standard operating mode
  // The ENS160 has three modes:
  // STANDARD — normal measurement mode, readings every second
  // LP       — low power mode, readings every 60 seconds
  // RESET    — resets the sensor
  ens160.setPWRMode(ENS160_STANDARD_MODE);

  Serial.println("Sensor warming up — takes up to 3 minutes");
  Serial.println("Status will show WARMING UP until ready");
  Serial.println("----------------------------------------");
  Serial.println("  Status        eCO2     TVOC    AQI");
  Serial.println("                ppm      ppb     1-5");
  Serial.println("----------------------------------------");
}

void loop() {
  // Read the sensor status — tells us if data is valid
  // 0 = Normal operation, data valid
  // 1 = Warming up — data unreliable
  // 2 = Initial startup — first 1 hour of first ever use
  // 3 = Invalid output — something wrong
  uint8_t status = ens160.getENS160Status();

  String statusStr;
  switch (status) {
    case 0: statusStr = "OK            "; break;
    case 1: statusStr = "Warming up    "; break;
    case 2: statusStr = "Initial start "; break;
    default: statusStr = "Invalid       "; break;
  }

  uint16_t eco2 = ens160.getECO2();
  uint16_t tvoc = ens160.getTVOC();
  uint8_t  aqi  = ens160.getAQI();

  Serial.print("  "); Serial.print(statusStr);
  Serial.printf("%-9u%-9u%u\n", eco2, tvoc, aqi);

  // AQI meaning for reference:
  // 1 = Excellent, 2 = Good, 3 = Moderate
  // 4 = Poor,      5 = Unhealthy

  delay(2000);
}