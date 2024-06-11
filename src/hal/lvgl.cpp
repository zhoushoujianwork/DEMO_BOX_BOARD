#include "lvgl.h"
#include <TFT_eSPI.h>

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * TFT_HEIGHT / 10];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

#if LV_USE_LOG != 1
/* Serial debugging */
void my_print(const char *buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

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
    // 黑色背景
    // lv_obj_set_style_bg_color(label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label, txt.c_str());      // 设置文本
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); // 居中显示
    lv_timer_handler();
    lv_tick_inc(1000);
    delay(2000);
}

static void set_speed(void *obj, int32_t v)
{
    lv_arc_set_value((lv_obj_t *)obj, (int16_t)v);

    lv_label_set_text_fmt(ui_speedText, "%d", int(v));
    if (v < 40)
    {
        lv_obj_set_style_arc_color(ui_speed, lv_color_hex(0x2274C2), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else if (v < 80)
    {
        lv_obj_set_style_arc_color(ui_speed, lv_color_hex(0x0A9F62), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else if (v < 120)
    {
        lv_obj_set_style_arc_color(ui_speed, lv_color_hex(0xEF1616), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else
    {
        lv_obj_set_style_arc_color(ui_speed, lv_color_hex(0xEF1616), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    };
}

// 开机初始化动画仪表展示。
void init_speed_dashboard(void *param)
{
    /* INITIALIZE AN ANIMATION
     *-----------------------*/
    lv_anim_t a;
    lv_anim_init(&a);

    /* MANDATORY SETTINGS
     *------------------*/
    lv_anim_set_exec_cb(&a, set_speed);
    // lv_anim_set_time(&a, 1000);
    lv_anim_speed_to_time(2, 0, 299);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, ui_speed);
    lv_anim_set_values(&a, 0, 299);
    lv_anim_set_repeat_count(&a, 1);
    /* START THE ANIMATION
     *------------------*/
    lv_anim_start(&a);
}

// 判断是否还在 init 动画展示阶段，是则不再展示动画
void sync_speed(double v)
{
    set_speed(ui_speed, int(v));
}

void lv_flash()
{
    // 设置速度
    sync_speed(get_gps_data()->speed);

    // 设置本地时间
    lv_label_set_text_fmt(ui_gpsTime, "%4d-%02d-%02d %02d:%02d:%02d", get_gps_data()->year, get_gps_data()->month, get_gps_data()->day, get_gps_data()->hour, get_gps_data()->minute, get_gps_data()->second);
    // 设置经纬度
    lv_label_set_text_fmt(ui_gpsText, "%s,%s", String(get_gps_data()->lng, 6), String(get_gps_data()->lat, 6));

    // 设置高度
    lv_label_set_text_fmt(ui_altitudeText, "%sm", String(get_gps_data()->altitude, 2));

    // 设置方向
    lv_label_set_text_fmt(ui_courseText, "%s", String(get_gps_data()->direction, 0));

    // 设置卫星数量
    lv_label_set_text_fmt(ui_satellitesText, "%d", int(get_gps_data()->satellites));

    float roll = map(get_imu_data()->roll, -10, 10, -90, 90);
    // roll调整角度
    // roll = roll;
    lv_label_set_text_fmt(ui_rollText, "%s", String(roll, 0));
    lv_img_set_angle(ui_roll, map(roll, -90, 90, 0 - 900, 3600 / 2 - 900));

    if (get_device_state()->battery)
        lv_label_set_text_fmt(ui_battery, "%2d", get_device_state()->battery);

    Serial.printf("debug: %s %4d-%02d-%02d %02d:%02d:%02d %s,%s %d %s %d\n", String(roll, 0),
                  get_gps_data()->year, get_gps_data()->month, get_gps_data()->day, get_gps_data()->hour, get_gps_data()->minute, get_gps_data()->second,
                  String(get_gps_data()->lng, 6), String(get_gps_data()->lat, 6), int(get_gps_data()->altitude), String(get_gps_data()->direction, 0), int(get_gps_data()->satellites));
}

#if LV_USE_LOG != 0
void my_print(const char *buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

void setup_lvgl()
{
    Serial.println("TFT w*h:" + String(TFT_WIDTH) + "*" + String(TFT_HEIGHT));
    Serial.println("SDA:" + String(TFT_MOSI) + " SCL:" + String(TFT_SCLK) + " CS:" + String(TFT_CS) + " DC:" + String(TFT_DC) + " RST:" + String(TFT_RST));

    lv_init();
#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print);
#endif

    tft.begin();
    tft.setRotation(TFT_ROTATION);

    String LVGL_Arduino = "Hello Arduino LVGL!";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * TFT_HEIGHT / 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_HEIGHT;
    disp_drv.ver_res = TFT_WIDTH;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    ui_init();
    int initial_value = 0; // 初始值
    init_speed_dashboard(&initial_value);
}
