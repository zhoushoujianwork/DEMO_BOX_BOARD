#include "lvgl.h"
#include <TFT_eSPI.h>

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * TFT_HEIGHT / 10];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

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

// 时速超过 一定值时，仪表盘变化：
// 低于 80 ，arc 变绿色，label 变绿色
// 超过 80，arc 变黄色，label 变黄色
// 超过 120，arc 变红色，label 变红色
void lv_set_speed(double speed)
{
    lv_arc_set_value(ui_speed, speed);
    lv_label_set_text_fmt(ui_speedText, "%d", int(speed));
    if (speed < 40)
    {
        lv_obj_set_style_arc_color(ui_speed, lv_color_hex(0x2274C2), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else if (speed < 80)
    {
        lv_obj_set_style_arc_color(ui_speed, lv_color_hex(0x0A9F62), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else if (speed < 120)
    {
        lv_obj_set_style_arc_color(ui_speed, lv_color_hex(0xEF1616), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else
    {
        lv_obj_set_style_arc_color(ui_speed, lv_color_hex(0xEF1616), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    };
}

void lv_set_gps()
{
    // 设置速度
    lv_set_speed(get_gps_data()->speed);

    // 设置本地时间
    lv_label_set_text_fmt(ui_gpsTime, "%4d-%02d-%02d %02d:%02d:%02d", get_gps_data()->year, get_gps_data()->month, get_gps_data()->day, get_gps_data()->hour, get_gps_data()->minute, get_gps_data()->second);
    Serial.printf("gps_time %4d-%02d-%02d %02d:%02d:%02d\n", get_gps_data()->year, get_gps_data()->month, get_gps_data()->day, get_gps_data()->hour, get_gps_data()->minute, get_gps_data()->second);
    // 设置经纬度
    lv_label_set_text_fmt(ui_gpsText, "%s,%s", String(get_gps_data()->lng, 6), String(get_gps_data()->lat, 6));

    // 设置高度
    lv_label_set_text_fmt(ui_altitudeText, "%sm", String(get_gps_data()->altitude, 2));

    // 设置方向
    lv_label_set_text_fmt(ui_courseText, "%s", String(get_gps_data()->direction, 0));

    // 设置卫星数量
    lv_label_set_text_fmt(ui_satellitesText, "%d", int(get_gps_data()->satellites));
}

void lv_set_imu()
{
    float roll = map(get_imu_data()->pitch, -10, 10, -90, 90);
    lv_label_set_text_fmt(ui_rollText, "%s", String(roll, 0));
    // obj – pointer to an image object
    // angle – rotation angle in degree with 0.1 degree resolution (0..3600: clock wise)
    lv_img_set_angle(ui_roll, map(roll, -90, 90, 0 - 900, 3600 / 2 - 900));
    Serial.println("roll: " + String(roll, 0));
}

void lv_set_battery()
{
    // 设置蓝牙连接状态
    // if (!get_device_state()->gps_state)

    // if (!get_device_state()->imu_state)
    if (get_device_state()->battery)
        lv_label_set_text_fmt(ui_battery, "%2d", get_device_state()->battery);

    // if (!get_device_state()->temperature)
}

// 初始化的时候自带 lv 刷新时间的设置
void lv_init_set_speed(double speed, double time)
{
    lv_task_handler();
    lv_set_speed(speed);
    // TOOD: 开启下面会导致初始化速度变慢。
    // lv_img_set_angle(ui_roll, speed);
    // lv_label_set_text_fmt(ui_rollText, "%d°", int(speed));
    // lv_img_set_angle(ui_course, speed * 10);
    // lv_label_set_text_fmt(ui_courseText, "%d°", int(speed));
    lv_tick_inc(time);
}

// 开机初始化动画仪表展示。
void init_speed_dashboard()
{
    // 初始化仪表盘
    for (int i = 0; i <= 299; i += 1)
    {
        lv_init_set_speed(i, 2);
    }

    for (int j = 299; j >= 0; j -= 1)
    {
        lv_init_set_speed(j, 2);
    }
    delay(10);
    // 重置仪表盘
    lv_init_set_speed(0, 2);
}

void setup_lvgl()
{
    Serial.println("TFT w*h:" + String(TFT_WIDTH) + "*" + String(TFT_HEIGHT));
    Serial.println("SDA:" + String(TFT_MOSI) + " SCL:" + String(TFT_SCLK) + " CS:" + String(TFT_CS) + " DC:" + String(TFT_DC) + " RST:" + String(TFT_RST));

    tft.init();
    tft.setRotation(TFT_ROTATION);
    tft.fillScreen(TFT_BLACK);   // 填充黑色背景
    tft.setTextColor(TFT_WHITE); // 设置文字颜色
    tft.setTextSize(2);          // 设置文字大小
    // 打印引脚
    tft.setCursor(10, 10); // 设置光标位置
    tft.print("hello, Arduino! " + String(TFT_WIDTH) + "x" + String(TFT_HEIGHT));
    tft.drawLine(0, 0, 319, 171, TFT_RED); // 绘制一条对角线
    delay(1000);

    lv_init();
    String LVGL_Arduino = "Hello Arduino LVGL!";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * TFT_HEIGHT / 10);
    /*Initialize the display*/

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = TFT_HEIGHT;
    disp_drv.ver_res = TFT_WIDTH;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Create simple label */
    // lv_demo_text(LVGL_Arduino);

    ui_init();
    init_speed_dashboard();
}
