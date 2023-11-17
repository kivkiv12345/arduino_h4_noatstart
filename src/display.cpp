
#include "utils.hpp"

#if USE_XBEE == 0

#include "display.h"

#include <Ticker.h>


#define DISPLAY_PERIOD 3000

typedef void (*display_function)();

DISPLAY_FUNC
void display_temp_and_irq(void);

void display_ticker_func(void) {

    /* TODO Kevin: __attribute__((section(""))) appears to be broken on PlatformIO/AVR
        at least for functions. */

    return;

    /**
	 * GNU Linker symbols. These will be autogenerate by GCC when using
	 * __attribute__((section("displayfuncs"))
	 */
    __attribute__((weak)) extern display_function __start_displayfuncs;
    __attribute__((weak)) extern display_function __stop_displayfuncs;

#ifdef DEBUGGING_SECTIONS
    // _nameeee.id = 2;
    USE_SERIAL.printf("A %u\n", _nameeee.id5);

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 16);
    // display.printf("%p\n", *__start_displayfuncs);
    display.printf("B %p\n", __start_displayfuncs);
    display.printf("C %p\n", &_nameeee);
    display.display();
#endif

    /* Check if there are no functions in the section */
    /* Both __start_displayfuncs and __stop_displayfuncs 
        should be NULL in this case. */
    if (__start_displayfuncs == __stop_displayfuncs && __start_displayfuncs == NULL)
        return;
    
    static display_function* current_displayfunc = &__start_displayfuncs;

#ifdef DEBUGGING_SECTIONS
    display.printf("D %p\n", &_nameeee);
    // display.printf("D\n");
    display.display();
#endif

    // Call the current display function
    (*current_displayfunc)();

    // Move to the next display function
    current_displayfunc++;

    // Check if we've reached the end of the section and wrap around
    if (current_displayfunc >= &__stop_displayfuncs)
        current_displayfunc = &__start_displayfuncs;
}

Ticker DisplayTicker(display_ticker_func, 1000);

int display_update(void) {
    DisplayTicker.update();
    return 0;
}

int display_init(void) {
    DisplayTicker.start();
    return 0;
}

#endif
