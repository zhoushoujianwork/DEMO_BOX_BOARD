#ifndef _EM_CONFIG_H_
#define _EM_CONFIG_H_

#include <Arduino.h>
#define LedPin 2 // LED引脚

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

// SPI 显示屏

#endif
