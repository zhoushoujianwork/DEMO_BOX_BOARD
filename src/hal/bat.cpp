#include "em_config.h"

void adc_init()
{
    pinMode(PIN_BAT_EN, OUTPUT);       // ADC使能引脚
    digitalWrite(PIN_BAT_EN, LOW);     // 关闭ADC
    analogReadResolution(PIN_BAT_BIT); // ADC采样次数，默认就是12位
}

int get_adc_volts()
{
    // int analogValue = analogRead(PIN_BATTERY_ADC);
    // Serial.printf("ADC analog value = %d volts = %d\n",analogValue,analogValue*2.6/4095.0);
    int analogVolts = analogReadMilliVolts(PIN_BAT_ADC);
    Serial.printf("ADC millivolts value = %d\t", analogVolts);
    return analogVolts;
}

void read_battery()
{
    long battery = map(get_adc_volts() * 2, 3300, 4200, 0, 100);
    Serial.printf("battery = %d\t", battery);
}

void setup_bat()
{
    adc_init();
}
