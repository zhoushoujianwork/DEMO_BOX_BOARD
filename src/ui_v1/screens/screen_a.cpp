#include "../ui.h"
/*
172*320 固定比例的图形化布局
*/

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
        lv_tick_inc(1);    // 延迟5ms，你可以根据需要调整这个值
    }
}

// 回调函数，用于更改对象的背景颜色
void set_bg_color(void *obj, int32_t v)
{
    lv_color_t color = (v % 2 == 0) ? lv_color_hex(0x05915D) : lv_color_hex(0xEF1616);
    lv_obj_set_style_bg_color((lv_obj_t *)obj, color, LV_PART_MAIN);
}

/*背景变红-恢复的危险闪烁动画，被调用的时候跳动 10 次背景*/
void _bg_danger_anim(void *obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 4000); // 变化时间
    lv_anim_set_values(&a, 0, 4);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 循环
    lv_anim_set_exec_cb(&a, set_bg_color);                 // 背景变色
    lv_anim_start(&a);
}

// 全局变量，用于跟踪背景闪烁动画是否运行
bool bg_danger_anim_running = false;

void _set_arc_value(int32_t v)
{
    lv_arc_set_value(ui_speed_arc, v);
    // Create a custom event
    lv_obj_send_event(ui_speed_arc, LV_EVENT_VALUE_CHANGED, NULL);
}

void _set_gps_all(gps_data_t data)
{
    _set_arc_value(data.speed);
    lv_obj_align_to(ui_speedText, ui_speed_arc, LV_ALIGN_CENTER, 0, 0);

    // 设置本地时间
    lv_label_set_text_fmt(label_gps_time, "%4d-%02d-%02d %02d:%02d:%02d",
                          data.year, data.month, data.day, data.hour, data.minute, data.second);
    lv_obj_align_to(ui_speed_arc, label_gps_time, LV_ALIGN_CENTER, 0, 0);
    // 设置经纬度
    lv_label_set_text_fmt(label_gps_latlon, "%s\n%s", String(data.lng, 6), String(data.lat, 6));
    lv_obj_align(label_gps_latlon, LV_ALIGN_CENTER, 0, 0);

    // // 设置高度
    // lv_label_set_text_fmt(ui_altitudeText, "%sm", String(get_gps_data()->altitude, 2));

    // // 设置方向
    // lv_label_set_text_fmt(ui_courseText, "%s", String(get_gps_data()->direction, 0));

    // 设置卫星数量
    lv_label_set_text_fmt(label_gps_nu, "%d", int(data.satellites));

    // float roll = map(get_imu_data()->roll, -10, 10, -90, 90);

    //     Serial.printf("debug: %d(bat) %s(roll) %4d-%02d-%02d %02d:%02d:%02d %s,%s %d %s %d\n", get_device_state()->battery, String(roll, 0),
    //                   get_gps_data()->year, get_gps_data()->month, get_gps_data()->day, get_gps_data()->hour, get_gps_data()->minute, get_gps_data()->second,
    //                   String(get_gps_data()->lng, 6), String(get_gps_data()->lat, 6), int(get_gps_data()->altitude), String(get_gps_data()->direction, 0), int(get_gps_data()->satellites));
}

// 如果 arc value 改变了 则更新文本
void _ui_event_speed(lv_event_t *e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e); // 获取传进来的用户数据label
    lv_obj_t *arc = (lv_obj_t *)lv_event_get_target(e);      // 获取触发的控件arc
    int32_t v = lv_arc_get_value(arc);
    lv_label_set_text_fmt(ui_speedText, "%d", v);
    lv_obj_align_to(ui_speedText, ui_speed_arc, LV_ALIGN_CENTER, 0, 0);
    if (v < 120)
    {
        if (bg_danger_anim_running)
        {
            lv_anim_del(cont, set_bg_color); // 删除动画
            Serial.println("stop bg anim");
            bg_danger_anim_running = false;
        }
    }
    else
    {
        if (!bg_danger_anim_running)
        {
            // _bg_danger_anim(cont);
            Serial.println("start bg anim");
            bg_danger_anim_running = true;
        }
    }

    // Serial.printf("event: %d, value: %d, text: %s\n", lv_event_get_code(e),
    //               lv_arc_get_value(arc), lv_label_get_text(label));
}

void static_screen(void)
{

    // Gird 布局
    static int32_t col_dsc[] = {lv_pct(45), lv_pct(45), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {12, 150, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    lv_obj_t *cont = lv_obj_create(NULL);
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_style_radius(cont, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x05915D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cont, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(cont, lv_color_hex(0x055088), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(cont, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_center(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_disp_load_scr(cont);

    lv_obj_t *left_cont = lv_obj_create(cont);
    lv_obj_set_grid_cell(left_cont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_style_bg_opa(left_cont, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(left_cont, lv_color_hex(0x05915D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(left_cont, lv_color_hex(0x055088), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(left_cont, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(left_cont, lv_pct(100), lv_pct(100));
    lv_obj_center(left_cont);

    lv_obj_t *right_cont = lv_obj_create(cont);
    lv_obj_set_grid_cell(right_cont, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_style_bg_opa(right_cont, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(right_cont, lv_color_hex(0x05915D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(right_cont, lv_color_hex(0x055088), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(right_cont, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(right_cont, lv_pct(100), lv_pct(100));
    lv_obj_center(right_cont);

    /* 速度 arc */
    ui_speed_arc = lv_arc_create(left_cont); // 创建圆弧
    lv_obj_center(ui_speed_arc);
    // lv_obj_set_size(ui_speed_arc, lv_pct(100), lv_pct(100));
    lv_arc_set_range(ui_speed_arc, 0, 299);                  // 设置圆弧的最大值
    lv_obj_remove_style(ui_speed_arc, NULL, LV_PART_KNOB);   // 移除圆弧手柄
    lv_obj_remove_flag(ui_speed_arc, LV_OBJ_FLAG_CLICKABLE); // 移除可点击
    lv_obj_align(ui_speed_arc, LV_ALIGN_CENTER, 0, 0);

    ui_speedText = lv_label_create(left_cont); // 创建显示圆弧值的文本
    lv_obj_set_style_text_color(ui_speedText, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_speedText, &lv_font_montserrat_42, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_speedText, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT); // 文本居中
    lv_obj_set_style_text_opa(ui_speedText, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui_speedText, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ui_speedText, "%03d", 0);
    // lv_obj_align_to(ui_speedText, left_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(ui_speed_arc, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *ui_speedUnit = lv_label_create(left_cont); // // 时速单位 km/h
    lv_label_set_text(ui_speedUnit, "km/h");
    lv_obj_remove_style_all(ui_speedUnit);
    lv_obj_set_style_text_color(ui_speedUnit, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_speedUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_speedUnit, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_align_to(ui_speedUnit, left_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_align(ui_speed_arc, LV_ALIGN_BOTTOM_MID, 0, 0);

    /* bluetooth */
    // 创建一个标签
    lv_obj_t *label = lv_label_create(cont);
    // 设置标签内容为 Bluetooth 符号
    lv_label_set_text(label, LV_SYMBOL_BLUETOOTH);
    // 创建一个新的样式
    static lv_style_t style_text;
    lv_style_init(&style_text);
    // 设置文本样式为白色
    lv_style_set_text_color(&style_text, lv_color_white());
    // 将样式应用到标签上
    lv_obj_add_style(label, &style_text, 0);
    // 右上角对齐
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -10, 5);

    /* battery */
    lv_obj_t *label_battery = lv_label_create(cont);
    lv_label_set_text(label_battery, LV_SYMBOL_BATTERY_3); // LV_SYMBOL_BATTERY_1 少电，LV_SYMBOL_BATTERY_3 多电
    lv_obj_add_style(label_battery, &style_text, 0);
    lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -30, 5);

    /* gps */
    lv_obj_t *label_gps = lv_label_create(cont);
    lv_label_set_text(label_gps, LV_SYMBOL_GPS);
    lv_obj_add_style(label_gps, &style_text, 0);
    lv_obj_align(label_gps, LV_ALIGN_TOP_LEFT, 10, 5);
    // GPS 数量显示
    label_gps_nu = lv_label_create(cont);
    lv_obj_add_style(label_gps_nu, &style_text, 0);
    lv_label_set_text_fmt(label_gps_nu, "%d", 0);
    lv_obj_set_style_text_font(label_gps_nu, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(label_gps_nu, LV_ALIGN_TOP_LEFT, 30, 5);

    /* gps_time */
    label_gps_time = lv_label_create(cont);
    lv_obj_add_style(label_gps_time, &style_text, 0);
    lv_label_set_text_fmt(label_gps_time, "%04d-%02d-%02d %02d:%02d:%02d", 2024, 1, 1, 1, 1, 1);
    lv_obj_set_style_text_font(label_gps_time, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(label_gps_time, LV_ALIGN_TOP_MID, 0, 5);

    /* gps Lat Lon */
    label_gps_latlon = lv_label_create(right_cont);
    lv_obj_add_style(label_gps_latlon, &style_text, 0);
    lv_label_set_text_fmt(label_gps_latlon, "%s\n%s", "lat", "lon");
    lv_obj_set_style_text_align(label_gps_latlon, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_gps_latlon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(label_gps_latlon, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label_gps_latlon, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(label_gps_latlon, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_gps_latlon, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(label_gps_latlon, LV_ALIGN_CENTER, 0, 0);

    /* EVENT */
    lv_obj_add_event_cb(ui_speed_arc, _ui_event_speed, LV_EVENT_VALUE_CHANGED, (void *)ui_speedText); // 添加事件回调

    _init_board();
    Serial.println("screen_a init ok");
}