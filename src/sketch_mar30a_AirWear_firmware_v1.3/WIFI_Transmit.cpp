#include "WiFi_Transmit.h"
#include <WiFi.h>

// ---- Update these to match your hotspot ----
static const char*    WIFI_SSID     = "iPhone";
static const char*    WIFI_PASSWORD = "LIGHTNING88";
static const char*    SERVER_HOST   = "172.20.10.2";
static const uint16_t SERVER_PORT   = 5000;
// --------------------------------------------

void WiFi_Init(void)
{
  Serial.print("WiFi: connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("WiFi: connected, IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("WiFi: connection FAILED — will retry in loop");
  }
}

bool WiFi_IsConnected(void)
{
  return WiFi.status() == WL_CONNECTED;
}

void WiFi_Transmit(const BMV080_Record &pm, const ENS160_Record &aq, const GPS_Record &gps)
{
  // If not connected, attempt reconnection before transmitting
  if (!WiFi_IsConnected())
  {
    Serial.println("WiFi: reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 5000)
    {
      delay(500);
    }
    if (!WiFi_IsConnected())
    {
      Serial.println("WiFi: reconnect failed — skipping transmit");
      return;
    }
    Serial.println("WiFi: reconnected");
  }

  WiFiClient client;
  if (!client.connect(SERVER_HOST, SERVER_PORT))
  {
    Serial.println("WiFi: failed to connect to server");
    return;
  }

  // JSON packet containing all three sensor streams
  // plus GPS. Sent as one complete object per transmission.
  // \n at the end is the line delimiter the Python script
  // uses to know a complete packet has arrived
  char json[768];
  snprintf(json, sizeof(json),
    "{"
    // timestamp
    "\"millis\":%lu,"

    // BMV080 particulate matter readings
    // µg/m³ = micrograms per cubic metre
    "\"pm1\":%.2f,"
    "\"pm25\":%.2f,"
    "\"pm10\":%.2f,"
    "\"pm_obstructed\":%s,"

    // ENS160 air quality readings
    // ppm = parts per million, ppb = parts per billion
    "\"eco2_ppm\":%u,"
    "\"tvoc_ppb\":%u,"
    "\"aqi\":%u,"
    "\"ens_status\":%u,"
    "\"ens_valid\":%s,"

    // GPS location
    "\"lat\":%.6f,"
    "\"lon\":%.6f,"
    "\"alt_m\":%.2f,"
    "\"speed_kmph\":%.2f,"
    "\"satellites\":%lu,"
    "\"gps_valid\":%s,"

    // GPS date and time
    "\"year\":%u,"
    "\"month\":%u,"
    "\"day\":%u,"
    "\"hour\":%u,"
    "\"minute\":%u,"
    "\"second\":%u"
    "}\n",

    // timestamp
    (unsigned long)pm.timestamp_ms,

    // BMV080
    pm.pm1_0,
    pm.pm2_5,
    pm.pm10,
    pm.obstructed ? "true" : "false",

    // ENS160
    aq.eco2_ppm,
    aq.tvoc_ppb,
    aq.aqi,
    aq.status,
    aq.data_valid ? "true" : "false",

    // GPS
    gps.valid_location ? gps.latitude  : 0.0,
    gps.valid_location ? gps.longitude : 0.0,
    gps.altitude_m,
    gps.speed_kmph,
    (unsigned long)gps.satellites,
    gps.valid_location ? "true" : "false",

    // date and time
    gps.year,
    gps.month,
    gps.day,
    gps.hour,
    gps.minute,
    gps.second
  );

  client.print(json);
  client.stop();

  Serial.print("WiFi TX: ");
  Serial.println(json);
}