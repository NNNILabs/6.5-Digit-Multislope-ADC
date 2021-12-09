// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------ //
// tester //
// ------ //

#define tester_wrap_target 0
#define tester_wrap 0

static const uint16_t tester_program_instructions[] = {
            //     .wrap_target
    0x4004, //  0: in     pins, 4                    
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program tester_program = {
    .instructions = tester_program_instructions,
    .length = 1,
    .origin = -1,
};

static inline pio_sm_config tester_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + tester_wrap_target, offset + tester_wrap);
    return c;
}
#endif

