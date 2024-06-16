#include "./ui.h"

void ui_flash()
{
    if (!get_device_state()->bleConnected)
    {
        Serial.println("lvgl return cause ble not connected");
        return;
    }
    // 设置GPS
    _set_gps_all(*get_gps_data());
}

void _ui_init(void)
{
    // _lv_example_grid_5();
    static_screen();
}
