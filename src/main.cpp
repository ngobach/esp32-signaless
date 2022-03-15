#include <Arduino.h>
#include "ui.h"

void setup()
{
    Serial.begin(9600);
    UI::setup();
}

void loop()
{
    UI::tick();
    delay(5);
}
