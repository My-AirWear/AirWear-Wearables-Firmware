#include "Display_ST7789.h"
#include "LVGL_Driver.h"
#include "ui.h"

#include "BMV080_PM.h"
#include "ENS160_AQ.h"
#include "GPS_Module.h"
#include "SD_Logger.h"
#include "WiFi_Transmit.h"

#include <stdio.h>

SET_LOOP_TASK_STACK_SIZE(60 * 1024);

static lv_coord_t pm25_history[10] = {0,0,0,0,0,0,0,0,0,0};

int PM25_To_AQHI_Proxy(float pm25)
{
  if (pm25 <= 6)  return 1;
  if (pm25 <= 12) return 2;
  if (pm25 <= 18) return 3;
  if (pm25 <= 25) return 4;
  if (pm25 <= 32) return 5;
  if (pm25 <= 39) return 6;
  if (pm25 <= 49) return 7;
  if (pm25 <= 59) return 8;
  if (pm25 <= 69) return 9;
  return 10;
}

void UI_Update_PM25(float pm25)
{
  if (ui_TextArea5)
  {
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", pm25);
    lv_textarea_set_text(ui_TextArea5, buf);
  }

  if (ui_Arc2)
  {
    lv_arc_set_range(ui_Arc2, 1, 10);
    lv_arc_set_value(ui_Arc2, PM25_To_AQHI_Proxy(pm25));
  }

  for (int i = 0; i < 9; i++)
  {
    pm25_history[i] = pm25_history[i + 1];
  }
  pm25_history[9] = (pm25 > 40.0f) ? 40 : (lv_coord_t)pm25;

  if (ui_Chart1 && ui_Chart1_series_1)
  {
    lv_chart_set_point_count(ui_Chart1, 10);
    lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 40);
    for (int i = 0; i < 10; i++)
    {
      ui_Chart1_series_1->y_points[i] = pm25_history[i];
    }
    lv_chart_refresh(ui_Chart1);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(200);
  Serial.println("ESP32-S3 AirWear v1.3 boot");

  LCD_Init();
  Lvgl_Init();
  ui_init();
  Serial.println("UI initialised");

  GPS_Init();
  Serial.println("GPS initialised");

  SD_Log_Init();
  WiFi_Init();

  // I2C scanner — both sensors share GPIO6/5
  Wire.begin(6, 5);
  Serial.println("Scanning I2C bus on GPIO6/5...");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  Found device at 0x%02X\n", addr);
    }
  }
  Serial.println("I2C scan done");

  BMV080_Init();
  ENS160_Init();

  Serial.println("All systems initialised — entering main loop");
}

void loop()
{
  Timer_Loop();
  GPS_Update();

  static unsigned long lastSample  = 0;
  static unsigned long lastGpsMsg  = 0;

  if (millis() - lastSample >= 2000)
  {
    lastSample = millis();

    BMV080_Record pm  = {};
    ENS160_Record aq  = {};
    GPS_Record    gps = {};

    bool pmOk  = BMV080_Read(pm);
    bool aqOk  = ENS160_Read(aq);
    GPS_Get_Record(gps);

    if (pmOk)
    {
      Serial.printf(
        "BMV080  PM1:%.1f PM2.5:%.1f PM10:%.1f %s\n",
        pm.pm1_0, pm.pm2_5, pm.pm10,
        pm.obstructed ? "OBSTRUCTED" : "OK"
      );
    }
    else
    {
      Serial.println("BMV080: no data yet");
    }

    if (aqOk)
    {
      Serial.printf(
        "ENS160  eCO2:%uppm TVOC:%uppb AQI:%u status:%u %s\n",
        aq.eco2_ppm, aq.tvoc_ppb, aq.aqi, aq.status,
        aq.data_valid ? "OK" : "WARMING UP"
      );
    }
    else
    {
      Serial.println("ENS160: no data yet");
    }

    if (pmOk)
    {
      UI_Update_PM25(pm.pm2_5);
    }

    if (pmOk || aqOk)
    {
      bool sdOk = SD_Log_All(pm, aq, gps);
      Serial.print("SD: ");
      Serial.println(sdOk ? "OK" : "FAIL");
    }

    if (pmOk && aqOk)
    {
      WiFi_Transmit(pm, aq, gps);
    }
  }

  if (millis() - lastGpsMsg >= 5000)
  {
    lastGpsMsg = millis();

    GPS_Record gps = {};
    GPS_Get_Record(gps);

    if (gps.valid_location)
    {
      Serial.printf(
        "GPS  lat:%.6f lon:%.6f sats:%lu age:%lums\n",
        gps.latitude,
        gps.longitude,
        (unsigned long)gps.satellites,
        (unsigned long)gps.age_ms
      );
    }
    else
    {
      Serial.println("GPS: searching for fix...");
    }
  }

  delay(5);
}