#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET LED_BUILTIN // 4
Adafruit_SSD1306 display(OLED_RESET);

bool isNumeric(const char *s, int len) {

    while (*s && len--)
    {
        if (*s < '0' || *s > '9')
            return false;
        ++s;
    }
    return true;
}