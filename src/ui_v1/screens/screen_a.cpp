#include "../ui.h"

static void set_angle(void *obj, int32_t v)
{
    lv_arc_set_value((lv_obj_t *)obj, v);
}

void ui_screen_a_init(void)
{
    lv_obj_t *ui_Screen_A = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen_A, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_radius(ui_Screen_A, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Screen_A, lv_color_hex(0x05915D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen_A, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Screen_A, lv_color_hex(0x055088), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_Screen_A, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_Screen_A, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Screen_A, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_disp_load_scr(ui_Screen_A);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_arc_color(&style, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_arc_width(&style, 2);

    lv_obj_t *ui_speed = lv_arc_create(ui_Screen_A);
    lv_obj_add_style(ui_speed, &style, LV_PART_KNOB | LV_STATE_DEFAULT);
    // lv_arc_set_rotation(ui_speed, 270); // 顺时针旋转
    // lv_arc_set_change_rate(ui_speed, 20);                // 每秒转动 20 度
    // lv_obj_remove_style(ui_speed, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    lv_arc_set_angles(ui_speed, 0, 120);
    lv_obj_remove_flag(ui_speed, LV_OBJ_FLAG_CLICKABLE); /*To not allow adjusting by click*/
    lv_obj_center(ui_speed);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui_speed);

    lv_anim_set_time(&a, 5000);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_playback_time(&a, 5000);

    lv_anim_set_var(&a, ui_speed);
    lv_anim_set_values(&a, 0, 80);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_path_ease_in_out(&a);
    lv_anim_set_exec_cb(&a, set_angle);

    lv_anim_start(&a);
}