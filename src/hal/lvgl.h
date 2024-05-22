#ifndef _LVGL_H_
#define _LVGL_H_
#include "em_config.h"
#include "em_device.h"

#include "ui/ui.h"

void setup_lvgl();

void lv_set_gps();

void lv_set_imu();

void lv_set_battery();

#endif