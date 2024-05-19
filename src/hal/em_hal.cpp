#include "em_hal.h"
#include "hal/bat.h"

void run_led(led_type_e type)
{
    switch (type)
    {
    case LED_CONNECT:
        digitalWrite(LedPin, 0);
        delay(100);
        digitalWrite(LedPin, 1);
        break;
    case LED_WARN:
        digitalWrite(LedPin, 0);
        delay(100);
        digitalWrite(LedPin, 1);
        delay(100);
        digitalWrite(LedPin, 0);
        delay(100);
        digitalWrite(LedPin, 1);
        delay(100);
        digitalWrite(LedPin, 0);
        delay(100);
        digitalWrite(LedPin, 1);
        break;
    case LED_PRINTER_START:
        digitalWrite(LedPin, 0);
        delay(100);
        digitalWrite(LedPin, 1);
        break;
    default:
        break;
    }
}

void read_all_hal()
{
    read_battery();
    // read_temperature();
    // read_paper_statue();
}