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

#define ms_wrap_target 12
#define ms_wrap 24

static const uint16_t ms_program_instructions[] = {
    0xa02b, //  0: mov    x, !null        side 0     
    0x80a0, //  1: pull   block           side 0     
    0x6040, //  2: out    y, 32           side 0     
    0x00c5, //  3: jmp    pin, 5          side 0     
    0x0008, //  4: jmp    8               side 0     
    0xe001, //  5: set    pins, 1         side 0     
    0x00c5, //  6: jmp    pin, 5          side 0     
    0x000c, //  7: jmp    12              side 0     
    0xe002, //  8: set    pins, 2         side 0     
    0x00cc, //  9: jmp    pin, 12         side 0     
    0x0008, // 10: jmp    8               side 0     
    0x80a0, // 11: pull   block           side 0     
            //     .wrap_target
    0xf002, // 12: set    pins, 2         side 1     
    0x10d0, // 13: jmp    pin, 16         side 1     
    0xf001, // 14: set    pins, 1         side 1     
    0x1055, // 15: jmp    x--, 21         side 1     
    0xff02, // 16: set    pins, 2         side 1 [15]
    0xbb42, // 17: nop                    side 1 [11]
    0xf001, // 18: set    pins, 1         side 1     
    0x108c, // 19: jmp    y--, 12         side 1     
    0x000b, // 20: jmp    11              side 0     
    0xff01, // 21: set    pins, 1         side 1 [15]
    0xba42, // 22: nop                    side 1 [10]
    0x108c, // 23: jmp    y--, 12         side 1     
    0x000b, // 24: jmp    11              side 0     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program ms_program = {
    .instructions = ms_program_instructions,
    .length = 25,
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

