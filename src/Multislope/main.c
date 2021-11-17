#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ms.pio.h"

const uint8_t MDAT = 17; // SPI MISO Pin
const uint8_t MCLK = 18; // SPI Clock Pin
const uint8_t M_EN = 16;

const uint8_t MUX_A0 = 22;
const uint8_t MUX_A1 = 26;
const uint8_t MUX_A2 = 27;


const uint8_t PWMA = 20;//24; // need to be in the same PWM slice
const uint8_t PWMB = 21;//25;
const uint8_t COMP = 19;

int main() {
    static const float pio_freq = 20000000; // 20Mhz - 100 slices in a 100KHz square wave

    // Choose PIO instance (0 or 1)
    PIO pio = pio0;

    // Get first free state machine in PIO 0
    uint sm = pio_claim_unused_sm(pio, true);

    // Add PIO program to PIO instruction memory. SDK will find location and
    // return with the memory offset of the program.
    uint offset = pio_add_program(pio, &ms_program);

    // Calculate the PIO clock divider
    float div = (float)clock_get_hz(clk_sys) / pio_freq;

    // Initialize the program using the helper function in our .pio file
    ms_program_init(pio, sm, offset, PWMA, COMP, div);

    // Start running our PIO program in the state machine
    pio_sm_set_enabled(pio, sm, true);
    // Do nothing
    while (true) {
        sleep_ms(1000);
    }
    //sm_config_set_jmp_pin();
}