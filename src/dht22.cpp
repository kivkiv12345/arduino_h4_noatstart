
#include "utils.hpp"

#if USE_XBEE == 0

#include <Arduino.h>

#include <Ticker.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "traffic_light.h"  // TODO Kevin: Include to get_irq_cnt(), should be refactored.
#include "display.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

static Ticker * DhtTicker;

static float temperature = 0;
static float relative_humidity = 0;

void dht_ticker_func(void) {

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        USE_SERIAL.println(F("Error reading temperature!"));
    }
    else {
        USE_SERIAL.print(F("Temperature: "));
        USE_SERIAL.print(event.temperature);
        USE_SERIAL.println(F("°C"));
    }
    temperature = event.temperature;

    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        USE_SERIAL.println(F("Error reading humidity!"));
    }
    else {
        USE_SERIAL.print(F("Humidity: "));
        USE_SERIAL.print(event.relative_humidity);
        USE_SERIAL.println(F("%"));
    }
    relative_humidity = event.relative_humidity;
}

int dht_update(void) {
    (*DhtTicker).update();
    return 0;
}

int dht_init(void) {
    dht.begin();
    sensor_t sensor;
    dht.humidity().getSensor(&sensor);

    /* Ticker interval is minumum delay for the DHT22 in milliseconds. */
    DhtTicker = new Ticker(dht_ticker_func, sensor.min_delay / 1000);
    DhtTicker->start();

    return 0;
}

#ifdef DEBUGGING_SECTIONS
DISPLAY_FUNC
volatile void display_temp_and_irq(void) {

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.printf("IRQ CNT: %d\n", get_irq_cnt());
    display.printf("TMP: %f°C\n", (double)temperature);
    display.display();

}

DISPLAY_FUNC
volatile void display_hum_and_irq(void) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.printf("IRQ CNT: %d\n", get_irq_cnt());
    display.printf("HUM: %f°C\n", (double)relative_humidity);
    display.display();
}
#endif

#endif