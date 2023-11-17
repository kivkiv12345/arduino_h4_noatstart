#pragma once

#define REDLIGHT_PIN 52
#define YELLOWLIGHT_PIN 50
#define GREENLIGHT_PIN 51

int get_irq_cnt(void);
int traffic_light_update(void);
int traffic_light_init(void);