#include "imu.h"
#include <Arduino.h>
#include <Wire.h>
#include <SensorQMI8658.hpp>

#define USE_WIRE

SensorQMI8658 qmi;

IMUdata acc;
IMUdata gyr;

void setup_imu()
{

#ifdef USE_WIRE
    // Using WIRE
    if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, QMI8658_SDA, QMI8658_SCL))
    {
        printf("Failed to find QMI8658 - check your wiring!\n");
        while (1)
        {
            delay(1000);
        }
    }
#else
    if (!qmi.begin(QMI8658_IMU_CS))
    {
        printf("Failed to find QMI8658 - check your wiring!\n");
        while (1)
        {
            delay(1000);
        }
    }
#endif

    /* Get chip id */
    printf("Device ID: 0x%X\n", qmi.getChipID());

    qmi.configAccelerometer(
        SensorQMI8658::ACC_RANGE_4G,
        SensorQMI8658::ACC_ODR_1000Hz,
        SensorQMI8658::LPF_MODE_0,
        true);

    qmi.configGyroscope(
        SensorQMI8658::GYR_RANGE_64DPS,
        SensorQMI8658::GYR_ODR_896_8Hz,
        SensorQMI8658::LPF_MODE_3,
        true);

    qmi.enableGyroscope();
    qmi.enableAccelerometer();

    qmi.dumpCtrlRegister();

    printf("Read data now...\n");
}

int calculate_angle(float x, float y, float z)
{
    float angle = atan2(y, x) * 180 / PI;
    if (angle < 0)
    {
        angle += 360;
    }
    return angle;
}

void loop_imu()
{
    if (qmi.getDataReady())
    {
        if (qmi.getAccelerometer(acc.x, acc.y, acc.z))
        {
            // 计算旋转角度，这里假设你已经有一个函数可以将IMU的姿态数据转换为角度
            int angle = calculate_angle(acc.x, acc.y, acc.z);

            // 更新对象的旋转角度
            set_angle(angle);
        }

        if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z))
        {
            // 处理陀螺仪数据...
        }
    }
}
