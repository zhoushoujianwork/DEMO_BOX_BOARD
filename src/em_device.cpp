#include "em_device.h"

device_state_t g_device_state;
gps_data_t g_gps_data;
imu_data_t g_imu_data;

device_state_t *get_device_state()
{
    return &g_device_state;
}

gps_data_t *get_gps_data()
{
    return &g_gps_data;
}

imu_data_t *get_imu_data()
{
    return &g_imu_data;
}

void set_device_gps_status(state_e status)
{
    g_device_state.gps_state = status;
}
void set_device_imu_status(state_e status)
{
    g_device_state.imu_state = status;
}

void init_device_state()
{
    memset(&g_device_state, 0, sizeof(device_state_t));
    g_device_state.imu_state = STATUS_LACK;
    g_device_state.gps_state = STATUS_LACK;
    g_device_state.battery = 13;
    g_device_state.temperature = 30;

    // gps
    memset(&g_gps_data, 0, sizeof(gps_data_t));
    g_gps_data.year = 2022;
    g_gps_data.month = 1;
    g_gps_data.day = 1;
    g_gps_data.hour = 0;
    g_gps_data.minute = 0;
    g_gps_data.second = 0;
    g_gps_data.centisecond = 0;
    g_gps_data.lat = 0;
    g_gps_data.lng = 0;
    g_gps_data.altitude = 0;
    g_gps_data.speed = 0;
    g_gps_data.direction = 0;
    g_gps_data.satellites = 0;

    // imu
    memset(&g_imu_data, 0, sizeof(imu_data_t));
    g_imu_data.ax = 0;
    g_imu_data.ay = 0;
    g_imu_data.az = 0;
    g_imu_data.gx = 0;
    g_imu_data.gy = 0;
    g_imu_data.gz = 0;
    g_imu_data.temperature = 0;
}