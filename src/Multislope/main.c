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

// #if MAINS_FREQ == 50
//     const uint32_t MScyclesPerPLC = 6000;
// #elif MAINS_FREQ == 60
//     const uint32_t MScyclesPerPLC = 5000;
// #endif

const uint32_t MScyclesPerPLC = 10;

struct getCoutnsPayload
{
    PIO pio;
    uint sm;
    uint32_t reqCounts;
    int64_t countSum;
    uint32_t NPLC;
};

uint32_t rundownCounts = 0;

int64_t get_counts(PIO pio, uint sm , uint32_t countNum){
    uint32_t counts = 0;
    pio_sm_put_blocking(pio, sm, countNum - 1);
    gpio_put(LED_PIN, true);
    counts = ~pio_sm_get_blocking(pio, sm);
    rundownCounts = ~pio_sm_get_blocking(pio, sm);
    gpio_put(LED_PIN, false);
    //printf("preRundownCounts: %u", preRundownCounts);
    return ((2*(int64_t)counts) - (int64_t)countNum);
}

bool measurement_timer_callback(struct repeating_timer *t) {
    void* payloadPtr = t->user_data;
    struct getCoutnsPayload *payload = (struct getCoutnsPayload *)payloadPtr;
    int64_t counts = get_counts(payload->pio, payload->sm, payload->reqCounts);
    int64_t finalCount = (14 * (int64_t)counts) + ((int64_t)rundownCounts - 32);
    payload->countSum = (payload->countSum) + finalCount;
    if(payload->NPLC > 0)
        payload->NPLC = (payload->NPLC)-1;
    // printf("Repeat at: %lld\n", time_us_64());
    // printf("requested counts: %d\n", payload->reqCounts);
    // printf("returned counts: %" PRId64 "\n", counts);
    // printf("rundownCounts: %d\n", rundownCounts);
    // printf("count accumulator: %" PRId64 "\n", payload->countSum);
    //printf("final count: %" PRIu64 "\n", finalCount);
    return true;
}

double get_counts_NPLC(PIO pio, uint sm , uint32_t NPLC){
    //printf("NPLC counts requested: %d\n", NPLC);
    struct getCoutnsPayload payload;
    payload.pio = pio;
    payload.sm = sm;
    payload.reqCounts = MScyclesPerPLC;
    payload.countSum = 0;
    payload.NPLC = NPLC;

    void* payloadPtr = &payload;
    struct repeating_timer timer;
    add_repeating_timer_us(-1 * mainsPeriodus * 2, measurement_timer_callback, payloadPtr, &timer);
    while (payload.NPLC > 0)
    {
        tight_loop_contents();
    }
    bool cancelled = cancel_repeating_timer(&timer);
    //printf("%" PRId64 "\n", payload.countSum);
    //printf("%d\n", payload.countSum);
    // for(; NPLC > 0; NPLC--){
    //     printf("NPLC: %d\n", NPLC);
    // }
    //bool cancelled = cancel_repeating_timer(&timer);
    return payload.countSum;
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
        // gpio_put(LED_PIN, true);
        // sleep_ms(500);
        // gpio_put(LED_PIN, false);
        // sleep_ms(500);
        uint32_t reqPLCCounts = 1;

        //configure PIO and start NPLC measurement
        //ms_program_init(pio, multislopeSM, multislopeOffset, PWMA, COMP, div, MEAS);
        
        //get_counts_NPLC(pio, multislopeSM, reqCounts);
        double counts = get_counts_NPLC(pio, multislopeSM, reqPLCCounts);
        printf("%" PRId64 "\n", counts);
        sleep_ms(1000);


        //ms_program_init(pio, multislopeSM, multislopeOffset, PWMA, COMP, div, MEAS);
        //uint32_t counts = get_counts(pio, multislopeSM, reqCounts);
        //printf("%d\n", counts);
    }
    
}
