// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// -- //
// ms //
// -- //

#define ms_wrap_target 14
#define ms_wrap 26

static const uint16_t ms_program_instructions[] = {
    0xa02b, //  0: mov    x, !null        side 0     
    0x80a0, //  1: pull   block           side 0     
    0x6040, //  2: out    y, 32           side 0     
    0x000e, //  3: jmp    14              side 0     
    0xe000, //  4: set    pins, 0         side 0     
    0x4020, //  5: in     x, 32           side 0     
    0xa04b, //  6: mov    y, !null        side 0     
    0xa02b, //  7: mov    x, !null        side 0     
    0xe002, //  8: set    pins, 2         side 0     
    0x0049, //  9: jmp    x--, 9          side 0     
    0x00c9, // 10: jmp    pin, 9          side 0     
    0xe001, // 11: set    pins, 1         side 0     
    0x4020, // 12: in     x, 32           side 0     
    0x0000, // 13: jmp    0               side 0     
            //     .wrap_target
    0xf002, // 14: set    pins, 2         side 1     
    0x10d2, // 15: jmp    pin, 18         side 1     
    0xf001, // 16: set    pins, 1         side 1     
    0x1057, // 17: jmp    x--, 23         side 1     
    0xff02, // 18: set    pins, 2         side 1 [15]
    0xbb42, // 19: nop                    side 1 [11]
    0xf001, // 20: set    pins, 1         side 1     
    0x108e, // 21: jmp    y--, 14         side 1     
    0x0004, // 22: jmp    4               side 0     
    0xff01, // 23: set    pins, 1         side 1 [15]
    0xba42, // 24: nop                    side 1 [10]
    0x108e, // 25: jmp    y--, 14         side 1     
    0x0004, // 26: jmp    4               side 0     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program ms_program = {
    .instructions = ms_program_instructions,
    .length = 27,
    .origin = -1,
};

static inline pio_sm_config ms_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + ms_wrap_target, offset + ms_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}

// Helper function (for use in C program) to initialize this PIO program
void ms_program_init(PIO pio, uint sm, uint offset, uint pin, uint input, float div, uint pin_MEAS) {
    // Sets up state machine and wrap target. This function is automatically
    pio_sm_config c = ms_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin_MEAS); //side set
    // Allow PIO to control GPIO pin (as output)
    pio_gpio_init(pio, pin);
    pio_gpio_init(pio, pin+1);
    pio_gpio_init(pio, pin_MEAS);      
    // set the pin for jump if pin high instruction
    sm_config_set_jmp_pin(&c, input); 
    // Connect pin to SET pin (control with 'set' instruction)
    sm_config_set_set_pins(&c, pin, 2);
    // Set the pin direction to output (in PIO)
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 2, true);      // 2 pins for PWM high and low
    pio_sm_set_consecutive_pindirs(pio, sm, pin_MEAS, 1, true); // 1 pin for MEAS pin
    // Set auto push to ISR
    sm_config_set_in_shift(&c, false, true, 32);
    // Set the clock divider for the state machine
    sm_config_set_clkdiv(&c, div);
    // Load configuration and jump to start of the program
    pio_sm_init(pio, sm, offset, &c);
}

#endif
