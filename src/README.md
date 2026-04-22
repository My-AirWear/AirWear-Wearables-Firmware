Source code (AirWear firmware) will live in this repo.

## Required Libraries for firmware

All libraries can be installed via Arduino IDE Library Manager (Sketch → Include Library → Manage Libraries) unless otherwise noted.

### Display & UI
- **Arduino_GFX** by Moon On Our Nation — display driver for ST7789
- **LVGL** version 8.3.x — graphics library (must be version 8, not 9)

### Sensors
- **DFRobot_BMV080** — particulate matter sensor (BMV080)
  - Must be installed manually from GitHub: https://github.com/DFRobot/DFRobot_BMV080
  - Use the bundled SDK v11.1.0 — do not replace SDK files
  - After installing, do not modify files in `src/esp32s3/`
- **DFRobot_ENS160** — air quality sensor (eCO2, TVOC, AQI)
  - Available in Library Manager, search "DFRobot ENS160"

### GPS
- **TinyGPSPlus** by Mikal Hart — NMEA sentence parsing
  - Available in Library Manager, search "TinyGPSPlus"

### Touch Controller
- **esp_lcd_touch_axs5106l** — touchscreen driver for Waveshare board
  - Install manually from Waveshare's GitHub or board support package

### Communication & Storage
- **WiFi** — built into ESP32 Arduino core, no installation needed
- **SD_MMC** — built into ESP32 Arduino core, no installation needed

---

## Arduino IDE Board Settings

These settings are required — incorrect settings will cause boot failures or compilation errors.

| Setting | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| Flash Size | **16MB (128Mb)** |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| PSRAM | OPI PSRAM |
| USB CDC on Boot | Enabled |
| Upload Mode | USB-OTG CDC (TinyUSB) |
