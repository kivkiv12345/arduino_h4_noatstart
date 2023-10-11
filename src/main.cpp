#undef USE_AVR_DEBUGGER
 
#include <Arduino.h>
 
#include <Ticker.h>
 
#if defined USE_AVR_DEBUGGER
#include "avr_debugger.h"
#include "avr8-stub.h"
#include "app_api.h"
#endif
 
#define USE_SERIAL Serial2
 
#define LED 12
 
static volatile bool Interrupt_Occured = false;
static volatile uint32_t Interrupt_Counter = 0;
//=======================================================================
void changeState()
{
    digitalWrite(LED, !(digitalRead(LED)));  //Invert Current State of LED
    Interrupt_Occured = true;
    Interrupt_Counter++;
}
 
Ticker Blinker(changeState, 1000);
 
void setup() {
    // put your setup code here, to run once:
    pinMode(LED, OUTPUT);
#if defined USE_AVR_DEBUGGER
    debug_init();
#endif
    delay(3000);
    USE_SERIAL.begin(115200);
 
    Blinker.start();
}
 
void loop() {
    Blinker.update();
    if (Interrupt_Occured)
    {
        Interrupt_Occured = false;
        USE_SERIAL.print("Interrupt Counter : ");
        USE_SERIAL.println(Interrupt_Counter);
    }
}