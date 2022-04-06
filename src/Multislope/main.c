#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "ms.pio.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define MAINS_FREQ 50                   // Hz

const uint8_t MCLK = 18;                // SPI Clock Pin
const uint8_t M_EN = 16;

const uint8_t MUX_A0 = 22;
const uint8_t MUX_A1 = 26;
const uint8_t MUX_A2 = 27;

const uint8_t MEAS = 17;

const uint8_t PWMA = 20;                // need to be next to each other
const uint8_t PWMB = 21;                // need to be next to each other
const uint8_t COMP = 19;

const uint8_t LED_PIN = 0;

const uint8_t RESGP = 28;
const uint8_t RESADC = 2;

const uint64_t mainsPeriodus = 1000000/MAINS_FREQ;

#if MAINS_FREQ == 50
    const uint32_t MScyclesPerPLC = 6000;
#elif MAINS_FREQ == 60
    const uint32_t MScyclesPerPLC = 5000;
#endif

uint32_t rundownCounts = 0;

int64_t get_counts(PIO pio, uint sm , uint32_t countNum){
    uint32_t counts = 0;
    pio_sm_put_blocking(pio, sm, countNum - 1);
    gpio_put(LED_PIN, true);
    counts = ~pio_sm_get_blocking(pio, sm);
    rundownCounts = ~pio_sm_get_blocking(pio, sm);
    gpio_put(LED_PIN, false);
    return ((2*(int64_t)counts) - (int64_t)countNum);
}

int main() {
    //set_sys_clock_khz(128000, true);                    // Set system clock to 128 Mhz to make the PIO clock frequency be evenly divisible by it
    set_sys_clock_khz(96000, true);  
    stdio_init_all();


    gpio_init(MUX_A0);
    gpio_init(MUX_A1);
    gpio_init(MUX_A2);
    gpio_init(MCLK);
    gpio_init(LED_PIN);

    gpio_set_dir(MCLK, GPIO_OUT);
    gpio_set_dir(MUX_A0, GPIO_OUT);
    gpio_set_dir(MUX_A1, GPIO_OUT);
    gpio_set_dir(MUX_A2, GPIO_OUT);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    sleep_us(10);
    gpio_put(MCLK, true);
    gpio_put(MUX_A0, false);
    gpio_put(MUX_A1, false);
    gpio_put(MUX_A2, false);

    // Choose PIO instance (0 or 1)
    PIO pio = pio0;

    // Get first free state machine in PIO 0
    uint multislopeSM = pio_claim_unused_sm(pio, true);

    // Add PIO program to PIO instruction memory. SDK will find location and
    // return with the memory offset of the program.
    uint multislopeOffset = pio_add_program(pio, &ms_program);

    const float div = 10;

    // Initialize the program using the helper function in our .pio file
    ms_program_init(pio, multislopeSM, multislopeOffset, PWMA, COMP, div, MEAS);


    // Start running our PIO program in the state machine
    pio_sm_set_enabled(pio, multislopeSM, true);
    



    while (true) {
        uint32_t reqPLCCounts = 1;
        uint32_t requestedCounts = 120000;

        int64_t counts = get_counts(pio, multislopeSM, requestedCounts);
        int64_t finalCount = (14 * (int64_t)counts) + ((int64_t)rundownCounts - 32);
        double volts = 20 * ((double)finalCount/(16 * requestedCounts));
        printf("%fV | counts:%" PRId64 "\n", volts, finalCount);
        //printf("%" PRId64 "\n", finalCount);
        sleep_ms(1000);
    }
    
}
