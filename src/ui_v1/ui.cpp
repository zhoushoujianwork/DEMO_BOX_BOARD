#include "./ui.h"

void ui_flash()
{
    if (!get_device_state()->bleConnected)
    {
        show_bluetooth(false);
        return;
    }

    show_bluetooth(true);
    show_battery(get_device_state()->battery);
    set_gps_all(*get_gps_data());
    _set_gyro_value(*get_imu_data());
}

void _ui_init(void)
{
    // _lv_example_grid_5();
    static_screen();
}
