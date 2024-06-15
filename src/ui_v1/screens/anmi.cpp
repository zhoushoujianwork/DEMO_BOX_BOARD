#include "../ui.h"

void set_arc_value(void *obj, int32_t v)
{
    lv_arc_set_value((lv_obj_t *)obj, (int16_t)v);
    lv_obj_send_event(ui_speed_arc, LV_EVENT_VALUE_CHANGED, NULL);
}

/*仪表速度初始化动画*/
void _arc_speed_anim(void *obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 2500);
    // lv_anim_path_ease_in(&a); // 使用缓入动画
    lv_anim_path_ease_out(&a); // 使用缓出动画
    // lv_anim_path_linear(&a);      // 使用线性动画
    // lv_anim_path_ease_in_out(&a); // 使用缓入缓出动画
    // lv_anim_path_bounce(&a);      // 使用反弹动画
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, 299);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_path_ease_in_out(&a);
    lv_anim_set_exec_cb(&a, set_arc_value);
    lv_anim_start(&a);
}
