#include "em_device.h"

device_state_t g_device_state;

device_state_t *get_device_state()
{
    return &g_device_state;
}

void set_device_gps_status(state_e status)
{
    g_device_state.gps_state = status;
}
void set_device_imu_status(state_e status)
{
    g_device_state.imu_state = status;
}

void set_read_ble_finish(bool finish)
{
    g_device_state.read_ble_finish = finish;
}

void init_device_state()
{
    memset(&g_device_state, 0, sizeof(device_state_t));
    g_device_state.imu_state = STATUS_LACK;
    g_device_state.gps_state = STATUS_LACK;
    g_device_state.battery = 13;
    g_device_state.temperature = 30;
    g_device_state.read_ble_finish = false;

    g_device_state.gps_data.year = 2022;
    g_device_state.gps_data.month = 1;
    g_device_state.gps_data.day = 1;
    g_device_state.gps_data.hour = 0;
    g_device_state.gps_data.minute = 0;
    g_device_state.gps_data.second = 0;
    g_device_state.gps_data.centisecond = 0;
    g_device_state.gps_data.lat = 0;
    g_device_state.gps_data.lng = 0;
    g_device_state.gps_data.altitude = 0;
    g_device_state.gps_data.speed = 0;
    g_device_state.gps_data.direction = 0;
    g_device_state.gps_data.satellites = 0;

    g_device_state.imu_data.ax = 0;
    g_device_state.imu_data.ay = 0;
    g_device_state.imu_data.az = 0;
    g_device_state.imu_data.gx = 0;
    g_device_state.imu_data.gy = 0;
    g_device_state.imu_data.gz = 0;
    g_device_state.imu_data.temperature = 0;
}