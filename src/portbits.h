#pragma once

#include <avr/io.h>
#include <assert.h>
#include <stdint.h>

/**
 * @brief Provides a very readable format for assigning register bits.
 */
typedef struct __attribute__((packed)) portbits_s {
    uint8_t bit0 : 1;
    uint8_t bit1 : 1;
    uint8_t bit2 : 1;
    uint8_t bit3 : 1;
    uint8_t bit4 : 1;
    uint8_t bit5 : 1;
    uint8_t bit6 : 1;
    uint8_t bit7 : 1;
} portbits_t;

/**
 * @brief Allows a register to be accessed as bits or a byte.
 */
typedef union bitfield_union_u {
    portbits_t bits;
    uint8_t byte;
} portbits_union_t;

static_assert(sizeof(portbits_union_t) == 1, "portbits_t should not be larger than a uint8_t");
static_assert(sizeof(portbits_t) == 1, "portbits_t should not be larger than a uint8_t");

#define BIT_CONF(_port) ((portbits_t *)&_port)

static inline void set_bit(uint8_t * port, uint8_t bit, uint8_t value) {

    assert(bit < 8);
    assert(value < 2);

    /* Using a switch/case is quite verbose, but it allows us to use our BIT_CONF() macro.*/
    /* TODO Kevin: port must be a pointer,
        bit I'm not sure if this will work when we have to dereference it for BIT_CONF().
        Alternatively, this entire function could be a macro. */
    switch (bit) {
        case 0: BIT_CONF(*port)->bit0 = value; break;
        case 1: BIT_CONF(*port)->bit1 = value; break;
        case 2: BIT_CONF(*port)->bit2 = value; break;
        case 3: BIT_CONF(*port)->bit3 = value; break;
        case 4: BIT_CONF(*port)->bit4 = value; break;
        case 5: BIT_CONF(*port)->bit5 = value; break;
        case 6: BIT_CONF(*port)->bit6 = value; break;
        case 7: BIT_CONF(*port)->bit7 = value; break;
    }
}

static inline uint8_t get_bit(uint8_t port, uint8_t bit) {

    assert(bit < 8);

    /* Using a switch/case is quite verbose, but it allows us to use our BIT_CONF() macro.*/
    switch (bit) {
        case 0: return BIT_CONF(port)->bit0;
        case 1: return BIT_CONF(port)->bit1;
        case 2: return BIT_CONF(port)->bit2;
        case 3: return BIT_CONF(port)->bit3;
        case 4: return BIT_CONF(port)->bit4;
        case 5: return BIT_CONF(port)->bit5;
        case 6: return BIT_CONF(port)->bit6;
        case 7: return BIT_CONF(port)->bit7;
    }
}

