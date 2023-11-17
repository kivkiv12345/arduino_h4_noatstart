#pragma once

#include <Arduino.h>
#include <stdbool.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define USE_XBEE 0

#define DISPLAY_ATTACHED

#define USE_SERIAL Serial2

extern Adafruit_SSD1306 display;

bool isNumeric(const char *s, int len);
void flashLed(int pin, int times, int wait);