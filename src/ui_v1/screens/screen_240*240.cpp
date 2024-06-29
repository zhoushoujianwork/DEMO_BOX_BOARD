#include "../ui.h"

static lv_obj_t *screen_a;
static lv_obj_t *left_top_cont;
static lv_obj_t *right_top_cont;
static lv_obj_t *left_cont;
static lv_obj_t *right_cont;

static lv_obj_t *label_gps_time;
static lv_obj_t *label_gps_nu;
static lv_obj_t *label_gps_dir;
static lv_obj_t *label_gps_alt;
static lv_obj_t *label_gps_latlon;
static lv_obj_t *ui_speed_arc;
static lv_obj_t *ui_imu_arc;
static lv_obj_t *label_gyro;
static lv_obj_t *ui_speedText;
static lv_obj_t *ui_speedUnit;

static lv_obj_t *label_bluetooth;
static lv_obj_t *label_battery;
// static lv_obj_t *label_gps;

void _flash_speed(int32_t v)
{
    lv_label_set_text_fmt(ui_speedText, "%d", v);
    lv_obj_align(ui_speedText, LV_ALIGN_CENTER, 0, lv_pct(18));
}

// 0-360° gps deg 转方向 N，E，S，W，NE，NW，SE，SW
// 地面航向角：以正北为0度，顺时针增加，
// 0：正北 45：东北 90：东 135：东南 180：正南 225：西南 270：西 315：西北
static const char *compass_str[] = {"N", "E", "S", "W", "NE", "NW", "SE", "SW"};
String get_compass_str(int32_t v)
{
    if (v < 0)
        v += 360;

    return compass_str[v / 45 % 8];
}

void _flash_gps_data(gps_data_t data)
{
    lv_label_set_text_fmt(label_gps_time, "%4d-%02d-%02d %02d:%02d:%02d",
                          data.year, data.month, data.day, data.hour, data.minute, data.second);
    lv_obj_align(label_gps_time, LV_ALIGN_CENTER, 0, 0);

    lv_label_set_text_fmt(label_gps_nu, "%2d", data.satellites);
    lv_obj_align(label_gps_nu, LV_ALIGN_CENTER, lv_pct(-23), lv_pct(-20));

    lv_label_set_text_fmt(label_gps_alt, "%sm", String(data.altitude, 0));
    lv_obj_align(label_gps_alt, LV_ALIGN_CENTER, lv_pct(-9), lv_pct(-20));

    lv_label_set_text_fmt(label_gps_dir, "%s", get_compass_str(data.direction));
    lv_obj_align(label_gps_dir, LV_ALIGN_CENTER, lv_pct(5), lv_pct(-20));

    lv_label_set_text_fmt(label_gps_latlon, "%s,%s", String(data.lng, 6), String(data.lat, 6));
    lv_obj_align(label_gps_latlon, LV_ALIGN_CENTER, 0, lv_pct(-10));
}

// 全局变量，用于跟踪背景闪烁动画是否运行
static bool bg_danger_anim_running = false;

// 回调函数，用于更改对象的背景颜色
void set_bg_color(void *obj, int32_t v)
{
    lv_color_t color = (v % 2 == 0) ? lv_color_hex(0x05915D) : lv_color_hex(0xEF1616);
    // Serial.printf("set_bg_color %d %x\n", v, color);
    lv_obj_set_style_bg_color((lv_obj_t *)obj, color, LV_PART_MAIN | LV_STATE_DEFAULT);
}

// 改变 arc值同时触发事件更新速度字符
void _set_arc_value(int32_t v)
{
    lv_arc_set_value(ui_speed_arc, v);
    // Create a custom event
    lv_obj_send_event(ui_speed_arc, LV_EVENT_VALUE_CHANGED, NULL);
}

void _bg_danger_anim(void *obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_values(&a, 0, 10);                         // 一共 20 个值，从 0 到 20
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 循环
    lv_anim_set_exec_cb(&a, set_bg_color);                 // 背景变色
    lv_anim_start(&a);
}

// 如果 arc_imu value 改变了 则更新文本
void _ui_event_imu(lv_event_t *e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e); // 获取传进来的用户数据label
    lv_obj_t *arc = (lv_obj_t *)lv_event_get_target(e);      // 获取触发的控件arc
    int32_t v = lv_arc_get_value(arc);
}

// 如果 arc value 改变了 则更新文本
void _ui_event_speed(lv_event_t *e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e); // 获取传进来的用户数据label
    lv_obj_t *arc = (lv_obj_t *)lv_event_get_target(e);      // 获取触发的控件arc
    int32_t v = lv_arc_get_value(arc);

    _flash_speed(v);
    if (v < 80)
    {
        if (bg_danger_anim_running)
        {
            lv_anim_del(screen_a, set_bg_color); // 删除动画
            // 恢复背景色
            lv_obj_set_style_bg_color((lv_obj_t *)screen_a, lv_color_hex(0x05915D), LV_PART_MAIN | LV_STATE_DEFAULT);
            Serial.println("stop bg anim");
            bg_danger_anim_running = false;
        }
    }
    else
    {
        if (!bg_danger_anim_running)
        {
            Serial.println("start bg anim");
            _bg_danger_anim(screen_a);
            bg_danger_anim_running = true;
        }
    }

    // Serial.printf("event: %d, value: %d, text: %s\n", lv_event_get_code(e),
    //               lv_arc_get_value(arc), lv_label_get_text(label));
}

void _init_board(void)
{
    // 从0增加到299
    for (int i = 0; i <= 299; i++)
    {
        _set_arc_value(i);
        lv_task_handler(); // 处理LVGL任务，更新屏幕
        lv_tick_inc(5);    // 延迟5ms，你可以根据需要调整这个值
    }

    // 从299减少到0
    for (int i = 299; i >= 0; i--)
    {
        _set_arc_value(i);
        lv_task_handler(); // 处理LVGL任务，更新屏幕
        lv_tick_inc(5);    // 延迟5ms，你可以根据需要调整这个值
    }
}

void _set_gyro_value(imu_data_t data)
{
#ifdef USE_DOT_FOR_GYRO
    // 设置文本基于当前位置开始位移,活动范围 64 像素内
    int32_t _roll = map(data.roll, -10, 10, -32, 32);
    int32_t _pitch = map(data.pitch, -10, 10, -32, 32);
    lv_obj_align(label_gyro, LV_ALIGN_CENTER, _roll, _pitch);
    lv_arc_set_value(ui_imu_arc, data.roll * 100);
#else
    lv_image_set_rotation(label_gyro, map(value, -10, 10, -1800 / 2, 1800 / 2));
#endif
    // Serial.printf("set_gyro_value %d\n", value);
}

void show_bluetooth(bool connected)
{
    if (connected)
    {
        // Serial.println("lvgl show bluetooth");
        lv_label_set_text(label_bluetooth, LV_SYMBOL_BLUETOOTH);
    }
    else
    {
        // Serial.println("lvgl show bluetooth close");
        lv_label_set_text(label_bluetooth, LV_SYMBOL_CLOSE);
    }
}

// 5个状态，分别是 20% 40% 60% 80% 100%
void show_battery(uint8_t percent)
{
    if (percent < 20)
    {
        lv_label_set_text(label_battery, LV_SYMBOL_BATTERY_EMPTY);
    }
    else if (percent < 40)
    {
        lv_label_set_text(label_battery, LV_SYMBOL_BATTERY_1);
    }
    else if (percent < 60)
    {
        lv_label_set_text(label_battery, LV_SYMBOL_BATTERY_2);
    }
    else if (percent < 80)
    {
        lv_label_set_text(label_battery, LV_SYMBOL_BATTERY_3);
    }
    else
    {
        lv_label_set_text(label_battery, LV_SYMBOL_BATTERY_FULL);
    }
}

void static_screen(void)
{

    /*Create a container with grid*/
    screen_a = lv_obj_create(NULL);
    // 设置为圆
    lv_obj_set_style_radius(screen_a, 250, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(screen_a, lv_color_hex(0x05915D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(screen_a, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(screen_a, lv_color_hex(0x055088), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(screen_a, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_size(screen_a, lv_pct(100), lv_pct(100));
    // lv_obj_set_size(screen_a, 240, 240);
    lv_disp_load_scr(screen_a);

    /* 速度 arc */
    ui_speed_arc = lv_arc_create(lv_screen_active()); // 创建圆弧
    lv_obj_center(ui_speed_arc);
    lv_arc_set_range(ui_speed_arc, 0, 299); // 设置圆弧的最大值
    lv_arc_set_value(ui_speed_arc, 0);
    lv_obj_set_size(ui_speed_arc, 220, 220);
    lv_arc_set_bg_angles(ui_speed_arc, 150, 30);
    lv_obj_remove_style(ui_speed_arc, NULL, LV_PART_KNOB);   // 移除圆弧手柄
    lv_obj_remove_flag(ui_speed_arc, LV_OBJ_FLAG_CLICKABLE); // 移除可点击
    lv_obj_align(ui_speed_arc, LV_ALIGN_CENTER, 0, 0);

    ui_imu_arc = lv_arc_create(lv_screen_active()); // 创建圆弧
    lv_obj_center(ui_imu_arc);
    lv_arc_set_range(ui_imu_arc, -1000, 1000); // 设置圆弧的最大值
    lv_arc_set_value(ui_imu_arc, 0);
    lv_obj_set_size(ui_imu_arc, 220, 220);
    lv_arc_set_bg_angles(ui_imu_arc, 40, 140);
    lv_arc_set_mode(ui_imu_arc, LV_ARC_MODE_SYMMETRICAL);  // 对称模式
    lv_obj_remove_style(ui_imu_arc, NULL, LV_PART_KNOB);   // 移除圆弧手柄
    lv_obj_remove_flag(ui_imu_arc, LV_OBJ_FLAG_CLICKABLE); // 移除可点击
    lv_obj_align(ui_imu_arc, LV_ALIGN_CENTER, 0, 0);

    ui_speedUnit = lv_label_create(ui_speed_arc); // // 时速单位 km/h
    lv_label_set_text(ui_speedUnit, "km/h");
    lv_obj_remove_style_all(ui_speedUnit);
    lv_obj_set_style_text_color(ui_speedUnit, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_speedUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_speedUnit, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(ui_speedUnit, LV_ALIGN_CENTER, 0, lv_pct(32)); // 居中

    ui_speedText = lv_label_create(ui_speed_arc); // 创建显示圆弧值的文本
    lv_obj_set_style_text_color(ui_speedText, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_speedText, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_speedText, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT); // 文本居中
    lv_obj_set_style_text_opa(ui_speedText, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui_speedText, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    _flash_speed(0);

    static lv_style_t style_text;
    lv_style_init(&style_text);
    lv_style_set_text_color(&style_text, lv_color_white());
    lv_style_set_text_font(&style_text, &lv_font_montserrat_18);
    lv_style_set_text_line_space(&style_text, 0);
    lv_style_set_text_letter_space(&style_text, 0);
    lv_style_set_text_opa(&style_text, LV_OPA_COVER);
    lv_style_set_text_align(&style_text, LV_TEXT_ALIGN_CENTER);

    /* bluetooth */
    label_bluetooth = lv_label_create(lv_screen_active());
    lv_label_set_text(label_bluetooth, LV_SYMBOL_BLUETOOTH);
    lv_obj_add_style(label_bluetooth, &style_text, 0);
    lv_obj_align(label_bluetooth, LV_ALIGN_CENTER, lv_pct(-14), lv_pct(-30));

    /* battery */
    label_battery = lv_label_create(lv_screen_active());
    lv_label_set_text(label_battery, LV_SYMBOL_BATTERY_EMPTY); // LV_SYMBOL_BATTERY_1 少电，LV_SYMBOL_BATTERY_3 多电
    lv_obj_add_style(label_battery, &style_text, 0);
    lv_obj_align(label_battery, LV_ALIGN_CENTER, lv_pct(14), lv_pct(-30));

    /* gps */
    // label_gps = lv_label_create(lv_screen_active());
    // lv_label_set_text(label_gps, LV_SYMBOL_GPS);
    // lv_obj_add_style(label_gps, &style_text, 0);
    // lv_obj_align(label_gps, LV_ALIGN_CENTER, lv_pct(-23), lv_pct(-20));
    // GPS 数量显示
    label_gps_nu = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_gps_nu, &style_text, 0);
    lv_obj_align(label_gps_nu, LV_ALIGN_CENTER, lv_pct(-23), lv_pct(-20));

    // autitude
    label_gps_alt = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_gps_alt, &style_text, 0);
    lv_label_set_text_fmt(label_gps_alt, "%sm", "99");
    lv_obj_align(label_gps_alt, LV_ALIGN_CENTER, lv_pct(-9), lv_pct(-20));

    // direction
    label_gps_dir = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_gps_dir, &style_text, 0);
    lv_label_set_text_fmt(label_gps_dir, "%s", "NE");
    lv_obj_align(label_gps_dir, LV_ALIGN_CENTER, lv_pct(5), lv_pct(-20));

    /* gps_time */
    label_gps_time = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_gps_time, &style_text, 0);
    lv_obj_align(label_gps_time, LV_ALIGN_CENTER, lv_pct(0), lv_pct(-0));
    /* gps Lat Lon */
    label_gps_latlon = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_gps_latlon, &style_text, 0);
    lv_obj_align(label_gps_latlon, LV_ALIGN_CENTER, lv_pct(0), lv_pct(-10));
    /* EVENT */
    lv_obj_add_event_cb(ui_speed_arc, _ui_event_speed, LV_EVENT_VALUE_CHANGED, &ui_speedText); // 添加事件回调

    _flash_gps_data(*get_gps_data()); // 刷新gps数据

/* 陀螺仪角度显示 */
#ifdef USE_DOT_FOR_GYRO
    // 用点模拟上下左右移动
    label_gyro = lv_label_create(lv_screen_active());
    lv_obj_set_size(label_gyro, 64, 64);
    lv_obj_set_style_text_align(label_gyro, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_gyro, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(label_gyro, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // red
    lv_obj_set_style_text_color(label_gyro, lv_color_hex(0xEF1616), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(label_gyro, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_gyro, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label_gyro, LV_SYMBOL_PLUS);
    lv_obj_align(label_gyro, LV_ALIGN_CENTER, 0, 0);
#else
    // 用图片模拟
    LV_IMAGE_DECLARE(LV_MOTO);
    label_gyro = lv_image_create(screen_a);
    lv_image_set_src(label_gyro, &LV_MOTO);
    lv_obj_set_style_opa(label_gyro, LV_OPA_100, 0);              // 透明度设置为 100
    lv_obj_set_style_image_recolor_opa(label_gyro, LV_OPA_30, 0); // 透明度设置为 100
#endif

    Serial.println("screen_a init ok");
    // init dashboard
    _init_board();
}

void set_gps_all(gps_data_t data)
{
    _set_arc_value(data.speed);
    // 设置本地时间
    _flash_gps_data(data);
}
