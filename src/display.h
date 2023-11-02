
/* Using this #define above a function will cause it to be called
     when the adafruit display should cycle to the next frame/picture. */
#define DISPLAY_FUNC \
    __attribute__((section("displayfuncs"))) /* We iterate this section while cycling the adafruit display. */ \
	__attribute__((aligned(1))) \
	__attribute__((used))  /* Prevents the compiler from optimizing the function away. \
                                Evidently doesn't prevent the linker from doing the same. */

int display_update(void);
int display_init(void);