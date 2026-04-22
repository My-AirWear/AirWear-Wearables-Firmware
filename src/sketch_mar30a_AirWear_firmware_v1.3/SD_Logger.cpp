#include "SD_Logger.h"
#include "FS.h"
#include "SD_MMC.h"

#ifdef CONFIG_IDF_TARGET_ESP32S3
int clk = 16;
int cmd = 15;
int d0  = 17;
int d1  = 18;
int d2  = 13;
int d3  = 14;
#endif

static const char *LOG_FILE = "/airwear_log.csv";
static bool sd_ready = false;

bool SD_Log_Init(void)
{
  if (!SD_MMC.setPins(clk, cmd, d0, d1, d2, d3))
  {
    Serial.println("SD: pin set failed");
    return false;
  }

  if (!SD_MMC.begin())
  {
    Serial.println("SD: card mount failed");
    return false;
  }

  if (SD_MMC.cardType() == CARD_NONE)
  {
    Serial.println("SD: no card attached");
    return false;
  }

  sd_ready = true;
  Serial.println("SD: card mounted");

  // Write CSV header if file doesn't exist yet
  // This runs once on first boot with a new card
  if (!SD_MMC.exists(LOG_FILE))
  {
    File f = SD_MMC.open(LOG_FILE, FILE_WRITE);
    if (!f)
    {
      Serial.println("SD: failed to create log file");
      return false;
    }

    f.println(
      "millis,"
      "pm1_0,pm2_5,pm10,pm_obstructed,"
      "eco2_ppm,tvoc_ppb,aqi,ens_status,ens_valid,"
      "lat,lon,alt_m,speed_kmph,satellites,gps_valid,gps_age_ms,"
      "year,month,day,hour,minute,second"
    );
    f.close();
    Serial.println("SD: CSV header written");
  }

  return true;
}

bool SD_Log_All(const BMV080_Record &pm, const ENS160_Record &aq, const GPS_Record &gps)
{
  if (!sd_ready) return false;

  File f = SD_MMC.open(LOG_FILE, FILE_APPEND);
  if (!f)
  {
    Serial.println("SD: failed to open log file for append");
    return false;
  }

  f.printf(
    // timestamp
    "%lu,"
    // BMV080 particulate data
    "%.2f,%.2f,%.2f,%u,"
    // ENS160 air quality data
    "%u,%u,%u,%u,%u,"
    // GPS location data
    "%.6f,%.6f,%.2f,%.2f,%lu,%u,%lu,"
    // GPS date and time
    "%u,%u,%u,%u,%u,%u\n",

    // timestamp value
    (unsigned long)pm.timestamp_ms,

    // BMV080 values
    pm.pm1_0,
    pm.pm2_5,
    pm.pm10,
    (uint8_t)pm.obstructed,

    // ENS160 values
    aq.eco2_ppm,
    aq.tvoc_ppb,
    aq.aqi,
    aq.status,
    (uint8_t)aq.data_valid,

    // GPS values
    gps.valid_location ? gps.latitude  : 0.0,
    gps.valid_location ? gps.longitude : 0.0,
    gps.altitude_m,
    gps.speed_kmph,
    (unsigned long)gps.satellites,
    (uint8_t)gps.valid_location,
    (unsigned long)gps.age_ms,

    // GPS date and time
    gps.year,
    gps.month,
    gps.day,
    gps.hour,
    gps.minute,
    gps.second
  );

  f.close();
  return true;
}