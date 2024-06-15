#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H
#include "lvgl.h"
#include "em_config.h"

static lv_obj_t *ui_Screen_A;
static lv_obj_t *ui_speed_arc;
static lv_obj_t *ui_speedText;

void _lv_example_grid_5(void);
void lv_example_grid_1(void);
void lv_example_event_4(void);
void static_screen(void);

extern void _set_arc_value(int32_t v);
extern void ui_init(void);
#endif
