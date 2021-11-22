#include <stdio.h>
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


uint32_t get_counts(PIO pio, uint sm , uint32_t countNum){
    countNum = countNum - 1;
    uint32_t counts = 0;
    pio_sm_put_blocking(pio, sm, countNum);
    pio_sm_set_enabled(pio, sm, true);
    pio_sm_get_blocking(pio, sm); // ignore first read, to get comparator out of saturation
    counts = pio_sm_get_blocking(pio, sm);
    pio_sm_set_enabled(pio, sm, false);
    return ~counts;
}



int main() {
    static const float pio_freq = 20000000; // 20Mhz - 100 slices in a 100KHz square wave


    stdio_init_all();

    gpio_init(MDAT);
    gpio_init(MUX_A0);
    gpio_init(MUX_A1);
    gpio_init(MUX_A2);
    gpio_init(MCLK);

    gpio_set_dir(MDAT, GPIO_OUT);
    gpio_set_dir(MCLK, GPIO_OUT);
    gpio_set_dir(MUX_A0, GPIO_OUT);
    gpio_set_dir(MUX_A1, GPIO_OUT);
    gpio_set_dir(MUX_A2, GPIO_OUT);
    
    gpio_put(MDAT, true);
    sleep_us(10);
    gpio_put(MCLK, true);
    gpio_put(MUX_A0, false);
    gpio_put(MUX_A1, false);
    gpio_put(MUX_A2, false);


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
    pio_sm_set_enabled(pio, sm, false);
    // Do nothing


    
    while (true) {
        sleep_ms(1000);
        uint32_t reqCounts = 0xFFFF;
        ms_program_init(pio, sm, offset, PWMA, COMP, div);
        uint32_t counts = get_counts(pio, sm, reqCounts);
        printf("Requested counts: %d; returned counts: %d\n", reqCounts, counts);
    }
}