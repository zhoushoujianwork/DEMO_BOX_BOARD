#include "em_config.h"

bool btn_state = false;          // 按键状态
bool long_press_start = false;   // 是否已经开始长按
unsigned long btn_last_time = 0; // 按键按下时间,单位毫秒 主要用于后续区分长按和短按

void check_btn()
{
    if (btn_state == false)
    {
        if (digitalRead(BtnPin) == LOW)
        {
            // 消除抖动
            delay(10);
            if (digitalRead(BtnPin) == LOW)
            {
                btn_state = true;
                btn_last_time = millis();
            }
        }
    }
    else
    {
        if (digitalRead(BtnPin) == HIGH)
        {
            if (millis() - btn_last_time > ShortPressTime)
            {
                Serial.println("long click free");
                long_press_start = false;
            }
            else
            {
                Serial.println("short click");
            }
            btn_state = false;
        }
        else
        {
            if (millis() - btn_last_time > ShortPressTime)
            {
                if (long_press_start == false)
                {
                    Serial.println("long press");
                    long_press_start = true;
                }
            }
        }
    }
}

void setup_btn()
{
    Serial.print(BtnPin);
    pinMode(BtnPin, INPUT);
    Serial.println(" Button Ready");
}

void loop_btn()
{
    check_btn();
}