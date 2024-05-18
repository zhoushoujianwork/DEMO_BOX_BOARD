#include "lvgl.h"
#include <TFT_eSPI.h>

// 设置显示屏宽度高度
#define TFT_WIDTH 320
#define TFT_HEIGHT 172

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * TFT_HEIGHT / 10];
int TFT_ROTATION = 3; // TFT显示屏旋转角度

/* Display flushing */
static void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp_drv);
}

static void lv_demo_text(String txt)
{
    /*Create a simple text, background is back,text is white*/
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, txt.c_str());      // 设置文本
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); // 居中显示
    lv_timer_handler();
    lv_tick_inc(1000);
    delay(2000);
}

void setup_lvgl()
{
    String LVGL_Arduino = "Hello Arduino LVGL!";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println(LVGL_Arduino);

    lv_init();

    tft.begin();                   // 初始化 TFT
    tft.setRotation(TFT_ROTATION); // 设置为横屏模式
    // tft.fillScreen(TFT_WHITE);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * TFT_HEIGHT / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Create simple label */
    lv_demo_text(LVGL_Arduino);

    ui_init();
    // lv_refr_now(NULL);

    // ShowSpeed();
}