#include "em_config.h"

void blinkLed()
{
  digitalWrite(LedPin, HIGH);
  delay(1000);
  digitalWrite(LedPin, LOW);
  delay(1000);
}

void setup_led()
{
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, LOW);

  blinkLed();
}

void loop_led()
{
  blinkLed();
}
