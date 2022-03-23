#ifndef IS_NATIVE
#include <Arduino.h>
#else
#include <unistd.h>
#include <cstdio>
#endif

#include "ui.h"

#ifndef IS_NATIVE
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
#else
int main() {
    printf("Program starting\n");
    UI::setup();
    printf("Program started\n");

    while (true)
    {
        UI::tick();
        usleep(5000);
    }

    printf("Exiting\n");
    return 0;
}
#endif