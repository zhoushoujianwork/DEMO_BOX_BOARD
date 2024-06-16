#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H
#include "lvgl.h"
#include "em_config.h"
#include "em_device.h"

void _lv_example_grid_5(void);
void lv_example_grid_1(void);
void lv_example_event_4(void);
void static_screen(void);

void show_battery(uint8_t percent);
void show_bluetooth(bool connected);

extern void _set_arc_value(int32_t v);
extern void _set_gps_all(gps_data_t data);
extern void _ui_init(void);
extern void ui_flash(void);
#endif
