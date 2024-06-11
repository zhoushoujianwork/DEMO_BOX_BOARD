#ifndef _LVGL_H_
#define _LVGL_H_
#include "em_config.h"
#include "em_device.h"

#include "ui/ui.h"

void setup_lvgl();
void sync_speed(double v);
void lv_flash();

#endif