#include "LVGL_Driver.h"
#include "Display_ST7789.h"
#include "esp_lcd_touch_axs5106l.h"
#include <Wire.h>

// Touch pins for Waveshare ESP32-S3-Touch-LCD-1.47
#define TOUCH_I2C_SDA 42
#define TOUCH_I2C_SCL 41
#define TOUCH_RST     47
#define TOUCH_INT     48

static TwoWire TouchWire = TwoWire(1);

static uint32_t screenWidth;
static uint32_t screenHeight;
static uint32_t bufSize;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf = nullptr;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

static void touchpad_read_cb(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
  touch_data_t touch_data;

  bsp_touch_read();
  bool touched = bsp_touch_get_coordinates(&touch_data);

  if (touched)
  {
    data->point.x = touch_data.coords[0].x;
    data->point.y = touch_data.coords[0].y;
    data->state = LV_INDEV_STATE_PRESSED;
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void Lvgl_Init(void)
{
  TouchWire.begin(TOUCH_I2C_SDA, TOUCH_I2C_SCL);
  bsp_touch_init(&TouchWire, TOUCH_RST, TOUCH_INT, gfx->getRotation(), gfx->width(), gfx->height());

  lv_init();

  screenWidth = gfx->width();
  screenHeight = gfx->height();
  bufSize = screenWidth * 40;

  disp_draw_buf = (lv_color_t *)heap_caps_malloc(
    bufSize * sizeof(lv_color_t),
    MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
  );

  if (!disp_draw_buf)
  {
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(
      bufSize * sizeof(lv_color_t),
      MALLOC_CAP_8BIT
    );
  }

  if (!disp_draw_buf)
  {
    Serial.println("LVGL draw buffer allocation failed!");
    while (1)
    {
      delay(1000);
    }
  }

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, bufSize);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read_cb;
  lv_indev_drv_register(&indev_drv);
}

void Timer_Loop(void)
{
  lv_timer_handler();
}