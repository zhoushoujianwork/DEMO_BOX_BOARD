#include "./ui.h"

/*

static void set_speed(void *obj, int32_t v)
{
    lv_arc_set_value((lv_obj_t *)obj, (int16_t)v);

    lv_label_set_text_fmt(ui_speedText, "%d", int(v));
    if (v < 40)
    {
        lv_obj_set_style_arc_color(ui_speed_arc, lv_color_hex(0x2274C2), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else if (v < 80)
    {
        lv_obj_set_style_arc_color(ui_speed_arc, lv_color_hex(0x0A9F62), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else if (v < 120)
    {
        lv_obj_set_style_arc_color(ui_speed_arc, lv_color_hex(0xEF1616), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else
    {
        lv_obj_set_style_arc_color(ui_speed_arc, lv_color_hex(0xEF1616), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    };
}

void lv_flash()
{
    if (!get_device_state()->bleConnected)
    {
        lv_label_set_text_fmt(ui_localtionText, "ble lost");
        return;
    }
    lv_label_set_text_fmt(ui_localtionText, "SuZhou, China");
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

    Serial.printf("debug: %d(bat) %s(roll) %4d-%02d-%02d %02d:%02d:%02d %s,%s %d %s %d\n", get_device_state()->battery, String(roll, 0),
                  get_gps_data()->year, get_gps_data()->month, get_gps_data()->day, get_gps_data()->hour, get_gps_data()->minute, get_gps_data()->second,
                  String(get_gps_data()->lng, 6), String(get_gps_data()->lat, 6), int(get_gps_data()->altitude), String(get_gps_data()->direction, 0), int(get_gps_data()->satellites));
}
*/

void ui_init(void)
{
    // _lv_example_grid_5();
    static_screen();
}
