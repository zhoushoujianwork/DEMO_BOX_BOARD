#include "../ui.h"
/*
172*320 固定比例的图形化布局

*/

/*仪表速度初始化动画*/
static void set_angle(void *obj, int32_t v)
{
    lv_arc_set_value((lv_obj_t *)obj, v);
    // Create a custom event
    lv_obj_send_event((lv_obj_t *)obj, LV_EVENT_VALUE_CHANGED, NULL);
}
void _arc_speed_anim(void *obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 15000);
    // lv_anim_set_playback_delay(&a, 80);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, 249);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_path_ease_in_out(&a);
    lv_anim_set_exec_cb(&a, set_angle);
    lv_anim_start(&a);
}
/*设置速度动画*/
void _set_ui_speed_anim(void *obj, int32_t v)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1500);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, v);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_path_ease_in_out(&a);
    lv_anim_set_exec_cb(&a, set_angle);
    lv_anim_start(&a);
}

// 回调函数，用于更改对象的背景颜色
static void set_bg_color(void *obj, int32_t v)
{
    lv_color_t color = (v % 2 == 0) ? lv_color_hex(0x05915D) : lv_color_hex(0xEF1616);
    lv_obj_set_style_bg_color((lv_obj_t *)obj, color, LV_PART_MAIN);
}

// 背景闪烁动画停止函数
void _bg_danger_anim_stop(void *obj)
{
    lv_anim_del(obj, set_bg_color); // 删除动画
}
// 全局变量，用于跟踪背景闪烁动画是否运行
bool bg_danger_anim_running = false;

/*背景变红-恢复的危险闪烁动画，被调用的时候跳动 10 次背景*/
void _bg_danger_anim(void *obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 4000);                            // 变化时间
    lv_anim_set_values(&a, 0, 20);                         // 一共 20 个值，从 0 到 20
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 循环
    lv_anim_set_path_cb(&a, lv_anim_path_linear);          // 使用线性路径
    lv_anim_set_exec_cb(&a, set_bg_color);                 // 背景变色
    lv_anim_start(&a);
}

// 如果 arc value 改变了 则更新文本
void _ui_event_speed(lv_event_t *e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e); // 获取传进来的用户数据label
    lv_obj_t *arc = (lv_obj_t *)lv_event_get_target(e);      // 获取触发的控件arc
    int32_t v = lv_arc_get_value(arc);
    lv_label_set_text_fmt(ui_speedText, "%d", v);
    if (v < 80)
    {
        lv_obj_set_style_arc_color(ui_speed_arc, lv_color_hex(0x2274C2), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else if (v < 120)
    {
        lv_obj_set_style_arc_color(ui_speed_arc, lv_color_hex(0x0A9F62), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        if (bg_danger_anim_running)
        {
            _bg_danger_anim_stop(ui_Screen_A);
            bg_danger_anim_running = false;
        }
    }
    else
    {
        lv_obj_set_style_arc_color(ui_speed_arc, lv_color_hex(0xEF1616), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        if (!bg_danger_anim_running)
        {
            _bg_danger_anim(ui_Screen_A);
            bg_danger_anim_running = true;
        }
    }
    Serial.printf("event: %d, value: %d, text: %s\n", lv_event_get_code(e),
                  lv_arc_get_value(arc), lv_label_get_text(label));
}

void static_screen(void)
{
    /* 屏幕 */
    ui_Screen_A = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen_A, LV_OBJ_FLAG_SCROLLABLE); /// 禁止屏幕滚动
    lv_obj_set_style_radius(ui_Screen_A, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Screen_A, lv_color_hex(0x05915D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen_A, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Screen_A, lv_color_hex(0x055088), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_Screen_A, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_Screen_A, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Screen_A, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_disp_load_scr(ui_Screen_A);

    /* 速度 arc */
    static lv_style_t speed_arc_style; // arc样式
    lv_style_init(&speed_arc_style);
    lv_style_set_arc_color(&speed_arc_style, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_arc_width(&speed_arc_style, 2);
    lv_style_set_arc_opa(&speed_arc_style, 255);                    // 设置透明度
    lv_style_set_bg_grad_color(&speed_arc_style, lv_color_black()); // 设置背景颜色
    lv_style_set_bg_color(&speed_arc_style, lv_color_white());      // 设置背景颜色
    static lv_style_t ui_label_style;                               // 文本样式
    lv_style_init(&ui_label_style);
    lv_style_set_text_color(&ui_label_style, lv_color_white());
    lv_style_set_text_opa(&ui_label_style, 255);
    lv_style_set_text_font(&ui_label_style, &lv_font_montserrat_42);
    ui_speedText = lv_label_create(ui_Screen_A); // 创建显示圆弧值的文本
    lv_obj_add_style(ui_speedText, &ui_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(ui_speedText, "%d", 0);
    lv_obj_center(ui_speedText);
    ui_speed_arc = lv_arc_create(ui_Screen_A); // 创建圆弧
    lv_obj_add_style(ui_speed_arc, &speed_arc_style, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(ui_speed_arc, NULL, LV_PART_KNOB);                                            // 移除圆弧手柄
    lv_arc_set_range(ui_speed_arc, 0, 299);                                                           // 设置圆弧角度范围
    lv_obj_remove_flag(ui_speed_arc, LV_OBJ_FLAG_CLICKABLE);                                          // 移除可点击
    lv_obj_center(ui_speed_arc);                                                                      // 圆弧居中
    lv_obj_add_event_cb(ui_speed_arc, _ui_event_speed, LV_EVENT_VALUE_CHANGED, (void *)ui_speedText); // 添加事件回调

    lv_obj_t *ui_speedUnit = lv_label_create(ui_Screen_A); // // 时速单位 km/h
    lv_label_set_text(ui_speedUnit, "km/h");
    lv_obj_remove_style_all(ui_speedUnit);
    lv_obj_set_style_text_color(ui_speedUnit, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_speedUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_speedUnit, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(ui_speedUnit, ui_speedText, LV_ALIGN_OUT_BOTTOM_MID, 0, 10); // 居中对齐, 上移 10 像素

    _arc_speed_anim(ui_speed_arc);
    // _bg_danger_anim(ui_Screen_A);
    Serial.println("screen_a init ok");
}