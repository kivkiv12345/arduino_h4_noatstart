#define USE_AVR_DEBUGGER

#include "utils.hpp"

#if USE_XBEE == 0

#include <Arduino.h>

#include <Ticker.h>

#include "dht22.hpp"
#include "traffic_light.h"
#include "display.h"

#if defined USE_AVR_DEBUGGER
#include "avr_debugger.h"
#include "avr8-stub.h"
#include "app_api.h"
#endif



// #define ESP8266

#define StopLineCount 30

#ifdef DISPLAY_ATTACHED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#endif

#ifdef CYCLE_DISPLAY
const unsigned long PeriodBeforeShift = 3000;
unsigned long StartMillis;
unsigned long CurrentMillis;
unsigned long FunctionPointerTableCounter = 0;
#endif

void ClearScreen(void) {
    display.clearDisplay();
    display.display();

    for (int LineCounter = 0; LineCounter < StopLineCount; LineCounter++)
    {
        printf("\n");
    }
    printf("ClearDisplay\n");
}

#ifdef CYCLE_DISPLAY
void WriteText(void) {
#ifdef DISPLAY_ATTACHED
    display.clearDisplay();
    // display.setCursor(10,10);
    display.setTextSize(2);
    display.setCursor(0, 16);
    display.println("Embedded Styrer !!!");
    display.display();
#endif
    printf("WriteText\n");
}

void DraweLine(void) {
#ifdef DISPLAY_ATTACHED
    display.clearDisplay();

    display.drawLine(0, 0, display.width() - 1, display.height() - 1, WHITE);
    display.drawLine(display.width() - 1, 0, 0, display.height() - 1, WHITE);

    display.display();
#endif
    printf("Drawline\n");
}

void DrawRectangle(void) {
#ifdef DISPLAY_ATTACHED
    display.clearDisplay();

    display.drawRect(100, 10, 20, 20, WHITE);
    display.fillRect(10, 10, 45, 15, WHITE);
    display.drawRoundRect(60, 20, 35, 35, 8, WHITE);

    display.display();
#endif
    printf("DrawRectangle\n");
}

void DrawCircle(void) {
#ifdef DISPLAY_ATTACHED
    display.clearDisplay();
    display.drawCircle(60, 30, 30, WHITE);
    display.fillCircle(50, 20, 5, WHITE);
    display.fillCircle(70, 20, 5, WHITE);
    display.display();
#endif
    printf("DrawCircle\n");
}

void DrawTriangle() {
#ifdef DISPLAY_ATTACHED
    display.clearDisplay();
    display.drawTriangle(24, 1, 3, 55, 45, 55, WHITE);
    display.fillTriangle(104, 62, 125, 9, 83, 9, WHITE);
    display.display();
#endif
    printf("DrawTriangle\n");
}

void WriteTextTeam() {
#ifdef DISPLAY_ATTACHED
    display.clearDisplay();
    // display.setCursor(10,10);
    display.setTextSize(2);
    display.setCursor(0, 16);
    display.println("h2pd041121 Styrer !!!");
    display.display();
#endif
    printf("WriteTexth3pd041121\n");
}


// Erklæring af en prototype for en funktion pointer. Funktions pointeren kan bruges til funktioner, der okke tager imod nogle parametre og ikke returnerer
// en parameter.
// Skal man drage en parallel til en funktions pointer i C indenfor C# verdenen, vil der være tale om en Delegate.
typedef void (*DisplayFunctionPointer)();

// En tabel af funktions pointere af typen beskerevet lige herover.
DisplayFunctionPointer DisplayFunctionPointerTable[] = {
    &ClearScreen,
    &WriteText,
    &DraweLine,
    &DrawRectangle,
    &DrawCircle,
    &DrawTriangle,
    &WriteTextTeam
};
#endif  // CYCLE_DISPLAY



void setup(void) {
    USE_SERIAL.begin(9600);

#ifdef USE_AVR_DEBUGGER
    debug_init();
    delay(3000);
#endif

#ifdef DISPLAY_ATTACHED
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextColor(WHITE);
#endif

#ifdef CYCLE_DISPLAY
    StartMillis = millis(); // initial start time

    (*DisplayFunctionPointerTable[0])(); // Start med den første funktion i vores Funktion Pointer Tabel så vi ikke starter med et tomt Display, hvis vi har et Display tilkoblet !!!
#endif  // CYCLE_DISPLAY

    pinMode(LED, OUTPUT);
#if defined USE_AVR_DEBUGGER
    debug_init();
#endif
    delay(3000);
    USE_SERIAL.begin(115200);

    traffic_light_init();
    dht_init();
    display_init();

    // initialize digital pin LED_BUILTIN as an output.
}

void loop(void) {

    traffic_light_update();
    dht_update();  // TODO Kevin: DHT22 seems to cause a button IRQ, maybe move to a different pin?
    display_update();

#ifdef CYCLE_DISPLAY
    CurrentMillis = millis(); // get the current time

    if (CurrentMillis - StartMillis >= PeriodBeforeShift)
    { // test whether the period has elapsed

        FunctionPointerTableCounter = ++FunctionPointerTableCounter % (sizeof(DisplayFunctionPointerTable) / sizeof(DisplayFunctionPointerTable[0]));
        // Kald af den "udpegede" funktion sker ved brug af funktions pointeren herunder. Linjen herunder er faktisk "blot" et kald af en funktion.
        // Dette kan godt være lidt svært at se, hvis ikke man har anvendt syntaksen her før. Men det turde være åbenlys, hvilke fordele man som
        // programmør opnår ved at bruge funktions pointere og tabeller af samme. Ens kode bliver meget let at vedligeholde og udvide.
        // Ønsker man at lave en ny funktion, der skal kaldes på linje med de allerede eksisterende funktioner, skal man blot implementere funktionen ét sted
        // i koden og så indsætte adressen på denne funktion i fuknktions pointer tabellen.
        // Eneste "begrænsning" ved den nye funktion er, at denne ikke tager imod nogle parametre og heller ikke returnerer en parameter.
        // Dette ser vi nærmere på i næste program !!!
        (*DisplayFunctionPointerTable[FunctionPointerTableCounter])();

        StartMillis = CurrentMillis;
    }
#endif

}

#endif