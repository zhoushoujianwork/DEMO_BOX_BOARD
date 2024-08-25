#ifndef _EM_CONFIG_H_
#define _EM_CONFIG_H_

#include <Arduino.h>
#define LedPin 17 // LED引脚

#define BtnPin 0           // 按键引脚
#define ShortPressTime 500 // 短按时间 单位毫秒

#define PIN_BAT_ADC 34 // 电池电压检测引脚 GPIO36 需要支持ADC的引脚
#define PIN_BAT_EN 4   // ADC使能引脚 GPIO4
#define PIN_BAT_BIT 12 // ADC采样次数 0-4095

// BLE
#define BLE_NAME "ESP32-MOTO"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"            // 自定义打印UUID
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b2600"     // 自定义打印UUID
#define CHARACTERISTIC_UUID_GPS "beb5483e-36e1-4688-b7f5-ea07361b2601" // 自定义打印UUID
#define CHARACTERISTIC_UUID_IMU "beb5483e-36e1-4688-b7f5-ea07361b2602" // 自定义打印UUID

#define USE_DOT_FOR_GYRO 1 // 是否使用点样式显示陀螺仪数据

#define ENABLE_IMU 0

// 设置微雪 QMI8658 六轴IMU
#define USE_QMI8658
#define QMI8658_SDA 6
#define QMI8658_SCL 7

// 定义IMU模块的引脚
#define PIN_IMU_SDA 19 // IMU模块接入 I2C SDA
#define PIN_IMU_SCL 20 // IMU模块 I2C SCL

#ifndef SENSOR_IRQ
#define QMI8658_IRQ 38
#endif

#define QMI8658_IMU_CS 5
#endif

#define TFT_HOR_RES 172
#define TFT_VER_RES 320
