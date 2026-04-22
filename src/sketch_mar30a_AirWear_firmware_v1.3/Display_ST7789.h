#ifndef DISPLAY_ST7789_H
#define DISPLAY_ST7789_H

#include <Arduino_GFX_Library.h>

// Board / display settings
#define TFT_ROTATION 0
#define TFT_BL       46

// Global display pointer for LVGL driver use
extern Arduino_GFX *gfx;

// Public init function
void LCD_Init(void);

#endif