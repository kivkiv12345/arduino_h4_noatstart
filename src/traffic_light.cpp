#include "utils.hpp"

#if USE_XBEE == 0

#include <Arduino.h>

#include <Ticker.h>


#include "traffic_light.h"

#define BUTTON_INTERRUPT_PIN 3

#define REDLIGHT_PBPIN PB1
#define YELLOWLIGHT_PBPIN PB2
#define GREENLIGHT_PBPIN PB3

#define USE_TIMER_ISR 1

enum light_states_e
{
    RED_LIGHT = 0,
    REDYELLOW_LIGHT = 1,
    GREEN_LIGHT = 2,
    YELLOW_LIGHT = 3,
};

typedef struct traffic_light_s
{
    int redlight_period = 3;
    int redyellowlight_period = 2;
    int greenlight_period = 5;
    int yellowlight_period = 1;

    light_states_e current_state;
    int time_in_state;
} traffic_light_t;

traffic_light_t traffic_light = {
    .redlight_period = 3,
    .redyellowlight_period = 2,
    .greenlight_period = 5,
    .yellowlight_period = 1,
};

static volatile int interrupt_occurred = 0;
void display_irq_cnt(void) {
    interrupt_occurred = 1;
}

void light_state_machine(void) {

    traffic_light.time_in_state++;

    switch (traffic_light.current_state) {
        case RED_LIGHT:

            PORTB |= (1<<REDLIGHT_PBPIN);  // | Turn on
            PORTB &= ~((1<<YELLOWLIGHT_PBPIN)|(1<<GREENLIGHT_PBPIN));  // & ~ Turn off

            //digitalWrite(REDLIGHT_PIN, 1);
            //digitalWrite(YELLOWLIGHT_PIN, 0);
            //digitalWrite(GREENLIGHT_PIN, 0);

            if (traffic_light.time_in_state > traffic_light.redlight_period)
            {
                traffic_light.time_in_state = 0;
                traffic_light.current_state = REDYELLOW_LIGHT;
            }

            break;
        case REDYELLOW_LIGHT:

            PORTB |= (1<<REDLIGHT_PBPIN)|(1<<YELLOWLIGHT_PBPIN);  // Turn on
            PORTB &= ~((1<<GREENLIGHT_PBPIN));  // Turn off

            //digitalWrite(REDLIGHT_PIN, 1);
            //digitalWrite(YELLOWLIGHT_PIN, 1);
            //digitalWrite(GREENLIGHT_PIN, 0);

            if (traffic_light.time_in_state > traffic_light.redyellowlight_period)
            {
                traffic_light.time_in_state = 0;
                traffic_light.current_state = GREEN_LIGHT;
            }

            break;
        case GREEN_LIGHT:

            PORTB |= (1<<GREENLIGHT_PBPIN);  // Turn on
            PORTB &= ~((1<<REDLIGHT_PBPIN)|(1<<YELLOWLIGHT_PBPIN));  // Turn off

            //digitalWrite(REDLIGHT_PIN, 0);
            //digitalWrite(YELLOWLIGHT_PIN, 0);
            //digitalWrite(GREENLIGHT_PIN, 1);

            if (traffic_light.time_in_state > traffic_light.greenlight_period)
            {
                traffic_light.time_in_state = 0;
                traffic_light.current_state = YELLOW_LIGHT;
            }

            break;

        default:
        case YELLOW_LIGHT:

            PORTB |= (1<<YELLOWLIGHT_PBPIN);  // Turn on
            PORTB &= ~((1<<REDLIGHT_PBPIN)|(1<<GREENLIGHT_PBPIN));  // Turn off

            //digitalWrite(REDLIGHT_PIN, 0);
            //digitalWrite(YELLOWLIGHT_PIN, 1);
            //digitalWrite(GREENLIGHT_PIN, 0);

            if (traffic_light.time_in_state > traffic_light.yellowlight_period)
            {
                traffic_light.time_in_state = 0;
                traffic_light.current_state = RED_LIGHT;
            }

            break;
    }
}

#ifndef USE_TIMER_ISR
Ticker LightTicker(light_state_machine, 10);
#else
ISR(TIMER4_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
    //generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)

    light_state_machine();
}
#endif

static void traffic_light_uart_protocol(void) {

    int num_chars = USE_SERIAL.available();
    char buf[4] = {0};
    buf[4] = '\0';
    if (num_chars < 3)
    {
        return;
    }

    for (size_t i = 0; i < 3; i++)
    {
        buf[i] = USE_SERIAL.read(); // read one byte from serial buffer;
    }

    if (!isNumeric(&buf[1], 2))
    {
        return;
    }

    /* TODO Kevin: This protocol could be 'improved' by using the 2-3 most significant bits to select light/display,
        and the remaining bits to select period.
        This allows every byte to be interpreted as a request, with no invalid packets. */

    switch (buf[0]) {
        case '0':
            traffic_light.redlight_period = atoi(&buf[1]);
            break;
        case '1':
            traffic_light.redyellowlight_period = atoi(&buf[1]);
            break;
        case '2':
            traffic_light.greenlight_period = atoi(&buf[1]);
            break;
        case '3':
            traffic_light.yellowlight_period = atoi(&buf[1]);
            break;

        default:
            break;
    }
}

static unsigned int irq_cnt = 0;
int get_irq_cnt(void) {
    return irq_cnt;
}

int traffic_light_update(void) {

    traffic_light_uart_protocol();

#ifndef USE_TIMER_ISR
    LightTicker.update();
#endif

    if (interrupt_occurred) {
        interrupt_occurred = 0;
        irq_cnt++;

        USE_SERIAL.printf("IRQ CNT : %d\n", irq_cnt);

#define IRQ_UPDATE_DISPLAY
#ifdef IRQ_UPDATE_DISPLAY
#ifdef DISPLAY_ATTACHED
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0, 16);
        display.printf("IRQ CNT : %d\n", irq_cnt);
        display.display();
#endif
#endif
    }

    return 0;
}

int traffic_light_init(void) {

    // Set traffic light LED pins to output.
    pinMode(50, OUTPUT);
    pinMode(51, OUTPUT);
    pinMode(52, OUTPUT);
#ifndef USE_TIMER_ISR
    LightTicker.start();
#else
    cli();//stop interrupts

    //set timer4 interrupt at 1Hz
    TCCR4A = 0;// set entire TCCR1A register to 0
    TCCR4B = 0;// same for TCCR1B
    TCNT4  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR4A = 15624/1;// = (16*10^6) / (1*1024) - 1 (must be <65536)
    // turn on CTC mode
    TCCR4B |= (1 << WGM12);
    // Set CS12 and CS10 bits for 1024 prescaler
    TCCR4B |= (1 << CS12) | (1 << CS10);  
    // enable timer compare interrupt
    TIMSK4 |= (1 << OCIE4A);

    sei();//allow interrupts
#endif

    pinMode(BUTTON_INTERRUPT_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BUTTON_INTERRUPT_PIN), display_irq_cnt, RISING); // trigger when button pressed, but not when released.

    return 0;
}

#endif
