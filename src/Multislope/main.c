#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "ms.pio.h"
#include "debugclock.pio.h"

const uint8_t MDAT = 17; // SPI MISO Pin
const uint8_t MCLK = 18; // SPI Clock Pin
const uint8_t M_EN = 16;

const uint8_t MUX_A0 = 22;
const uint8_t MUX_A1 = 26;
const uint8_t MUX_A2 = 27;


const uint8_t PWMA = 20;//24; // need to be in the same PWM slice
const uint8_t PWMB = 21;//25;
const uint8_t COMP = 19;

const uint LED_PIN = 25;

const uint8_t RESGP = 28;
const uint8_t RESADC = 2;

const uint8_t DBCLK = 15;

uint32_t get_counts(PIO pio, uint sm , uint32_t countNum){
    countNum = countNum - 1;
    uint32_t counts = 0;
    pio_sm_put_blocking(pio, sm, countNum);
    pio_sm_set_enabled(pio, sm, true);
    //pio_sm_get_blocking(pio, sm); // ignore first read, to get comparator out of saturation
    counts = pio_sm_get_blocking(pio, sm);
    sleep_ms(10); // remove later
    pio_sm_set_enabled(pio, sm, false);
    return ~counts;
}


// void RES0_handler(){
//     irq_clear(PIO0_IRQ_0);
//     ADCResults0[ADResCounter0] = adc_read();
//     t1[ADResCounter0] = get_absolute_time();
//     ADResCounter0++;
//     printf("IRQ_0");
// }

// void RES1_handler(){
//     irq_clear(PIO0_IRQ_1);
//     ADCResults1[ADResCounter1] = adc_read();
//     t2[ADResCounter1] = get_absolute_time();
//     ADResCounter1++;
//     printf("IRQ_1");
// }
absolute_time_t t1;
absolute_time_t t2;
absolute_time_t t3;
absolute_time_t t4;

uint16_t ADCRes_1;
uint16_t ADCRes_2;
uint16_t ADCRes_3;
uint16_t ADCRes_4;

bool x = 0;
bool y = 0;
void pio_irh() {
  if (pio0_hw->irq & 1) {
    pio0_hw->irq = 1;
    if(!x){
        t1 = get_absolute_time();
        ADCRes_1 = adc_read();
    }else{
        t2 = get_absolute_time();
        ADCRes_2 = adc_read();
    }
    // PIO0 IRQ0 fired
    //printf("IRQ_0 %lld\n", to_us_since_boot(get_absolute_time()));
    x = 1;
  } else if (pio0_hw->irq & 2) {
    pio0_hw->irq = 2;
    if(!y){
        t3 = get_absolute_time();
        ADCRes_3 = adc_read();
    }else{
        t4 = get_absolute_time();
        ADCRes_4 = adc_read();
    }
    y = true;
    //printf("IRQ_1 %lld\n", to_us_since_boot(get_absolute_time()));
    // PIO0 IRQ1 fired
    
  }
}

int main() {
    set_sys_clock_khz(128000, true);            // Set system clock to 128 Mhz to make the PIO clock frequency be evenly divisible by it
    stdio_init_all();
    static const float pio_freq = 200000 * 32;  // 200khz * 32 cycles per count


    

    adc_init();
    adc_gpio_init(RESGP);
    adc_select_input(RESADC);

    gpio_init(MDAT);
    gpio_init(MUX_A0);
    gpio_init(MUX_A1);
    gpio_init(MUX_A2);
    gpio_init(MCLK);
    gpio_init(LED_PIN);

    gpio_set_dir(MDAT, GPIO_OUT);
    gpio_set_dir(MCLK, GPIO_OUT);
    gpio_set_dir(MUX_A0, GPIO_OUT);
    gpio_set_dir(MUX_A1, GPIO_OUT);
    gpio_set_dir(MUX_A2, GPIO_OUT);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    gpio_put(MDAT, true);
    sleep_us(10);
    gpio_put(MCLK, true);
    gpio_put(MUX_A0, false);
    gpio_put(MUX_A1, false);
    gpio_put(MUX_A2, false);


    // Choose PIO instance (0 or 1)
    PIO pio = pio0;

    // Get first free state machine in PIO 0
    uint multislopeSM = pio_claim_unused_sm(pio, true);
    uint debugclockSM = pio_claim_unused_sm(pio, true);
    // Add PIO program to PIO instruction memory. SDK will find location and
    // return with the memory offset of the program.
    uint multislopeOffset = pio_add_program(pio, &ms_program);
    uint debugclockOffset = pio_add_program(pio, &dbclk_program);
    // Calculate the PIO clock divider
    float div = (float)clock_get_hz(clk_sys) / pio_freq;

    // Initialize the program using the helper function in our .pio file
    ms_program_init(pio, multislopeSM, multislopeOffset, PWMA, COMP, div);
    dbclk_program_init(pio, debugclockSM, debugclockOffset, DBCLK, ((float)clock_get_hz(clk_sys) / (pio_freq * 4)));
    // Start running our PIO program in the state machine
    pio_sm_set_enabled(pio, debugclockSM, true);
    pio_sm_set_enabled(pio, multislopeSM, false);
    // Do nothing

    // irq_set_exclusive_handler(PIO0_IRQ_0, RES0_handler);
    // irq_set_enabled(PIO0_IRQ_0, true);

    // irq_set_exclusive_handler(PIO0_IRQ_1, RES1_handler);
    // irq_set_enabled(PIO0_IRQ_1, true);
    
    // Enable IRQ0 & 1 on PIO0
    irq_set_exclusive_handler(PIO0_IRQ_0, pio_irh);
    irq_set_enabled(PIO0_IRQ_0, true);
    pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;



    while (true) {
        gpio_put(LED_PIN, true);
        sleep_ms(500);
        gpio_put(LED_PIN, false);
        sleep_ms(500);
        uint32_t reqCounts = 10;
        // TO DO rest t1-t2 struct here and ADCResults
        // memset(ADCResults0, 0, sizeof(ADCResults0));
        // memset(ADCResults1, 0, sizeof(ADCResults1));    
        ms_program_init(pio, multislopeSM, multislopeOffset, PWMA, COMP, div);
        uint32_t counts = get_counts(pio, multislopeSM, reqCounts);
        printf("Requested counts: %d; returned counts: %d\n", reqCounts, counts);
        printf("IRQ1 time 1: %lld; ADC: %X\n", to_us_since_boot(t1), ADCRes_1);
        printf("IRQ1 time 2: %lld; ADC: %X\n", to_us_since_boot(t2), ADCRes_2);
        printf("IRQ2 time 1: %lld; ADC: %X\n", to_us_since_boot(t3), ADCRes_3);
        printf("IRQ2 time 2: %lld; ADC: %X\n", to_us_since_boot(t4), ADCRes_4);
        // uint64_t time = to_us_since_boot(get_absolute_time());
        // printf("%lld", time);
        // printf("\n");
        // for(int i = 0;i < 5; i++){
        //     printf("%lld\n", to_us_since_boot(t[i]));
        // }
        // printf("Beginning of measurement ADC array results (Rising):\n");
        // for (uint8_t i = 0; i < sizeof(ADCResults0)/sizeof(uint16_t); i++)
        // {
        //     printf("#%d, 0x%x; ", i, ADCResults0[i]);
        // }
        // printf("\nEnd of measurement ADC array results (Rising)");
        // printf("Beginning of measurement ADC array results (Falling):\n");
        // for (uint8_t i = 0; i < sizeof(ADCResults1)/sizeof(uint16_t); i++)
        // {
        //     printf("#%d, 0x%x", i, ADCResults1[i]);
        // }
        // printf("\nEnd of measurement ADC array results (Falling)");
        // printf("Beginning of difference Array:\n");
        // for (uint8_t i = 0; i < sizeof(t1)/sizeof(absolute_time_t); i++)
        // {
        //     int64_t delta = absolute_time_diff_us(t1[i], t2[i]);
        //     printf("#%d, %duS; ", i, delta);
        // }
        // printf("\nEnd of difference Array");
    }
}