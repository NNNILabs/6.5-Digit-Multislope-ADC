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

#define ms_wrap_target 5
#define ms_wrap 27

#define ms_SkippedCycles 9

static const uint16_t ms_program_instructions[] = {
    0xe049, //  0: set    y, 9                       
    0xa02b, //  1: mov    x, !null                   
    0x0005, //  2: jmp    5                          
    0xc000, //  3: irq    nowait 0                   
    0x0008, //  4: jmp    8                          
            //     .wrap_target
    0xee02, //  5: set    pins, 2                [14]
    0x0063, //  6: jmp    !y, 3                      
    0xa142, //  7: nop                           [1] 
    0x00cc, //  8: jmp    pin, 12                    
    0xe001, //  9: set    pins, 1                    
    0x004d, // 10: jmp    x--, 13                    
    0x0019, // 11: jmp    25                         
    0xa142, // 12: nop                           [1] 
    0xba42, // 13: nop                           [26]
    0xbf42, // 14: nop                           [31]
    0xe001, // 15: set    pins, 1                    
    0x0097, // 16: jmp    y--, 23                    
    0xc001, // 17: irq    nowait 1                   
    0x4020, // 18: in     x, 32                      
    0xa02b, // 19: mov    x, !null                   
    0x80a0, // 20: pull   block                      
    0x6040, // 21: out    y, 32                      
    0x0018, // 22: jmp    24                         
    0xa542, // 23: nop                           [5] 
    0x001b, // 24: jmp    27                         
    0xe001, // 25: set    pins, 1                    
    0x0011, // 26: jmp    17                         
    0xac42, // 27: nop                           [12]
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program ms_program = {
    .instructions = ms_program_instructions,
    .length = 28,
    .origin = -1,
};

static inline pio_sm_config ms_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + ms_wrap_target, offset + ms_wrap);
    return c;
}

// Helper function (for use in C program) to initialize this PIO program
void ms_program_init(PIO pio, uint sm, uint offset, uint pin, uint input, float div) {
    // Sets up state machine and wrap target. This function is automatically
    pio_sm_config c = ms_program_get_default_config(offset);
    // Allow PIO to control GPIO pin (as output)
    pio_gpio_init(pio, pin);
    pio_gpio_init(pio, pin+1);
    // set the pin for jump if pin high instruction
    sm_config_set_jmp_pin(&c, input); 
    // Connect pin to SET pin (control with 'set' instruction)
    sm_config_set_set_pins(&c, pin, 2);
    // Set the pin direction to output (in PIO)
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 2, true);
    // Set auto push and pull from OSR and ISR
    sm_config_set_in_shift(&c, false, true, 32);
    //sm_config_set_out_shift(&c, false, false, 32);
    // Set the clock divider for the state machine
    sm_config_set_clkdiv(&c, div);
    // Load configuration and jump to start of the program
    pio_sm_init(pio, sm, offset, &c);
}

#endif

