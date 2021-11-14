#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

uint multiSlopeFreq = 200000; // clock frequency for multislope


const uint8_t MDAT = 17; // SPI MISO Pin
const uint8_t MCLK = 18; // SPI Clock Pin
const uint8_t M_EN = 16;

const u_int8_t MUX_A0 = 22;
const u_int8_t MUX_A1 = 26;
const u_int8_t MUX_A2 = 27;


int main() {
    stdio_init_all();

    uint duty_cycle = 50;

    uint nsec_period = 1000000000/multiSlopeFreq; // compute the period in nanoseconds
    uint wrap_num = nsec_period/8; // (Optimize, by getting the actual frequency and using that for math) since the CPU frequency is 125MHz - one iteration of the PWM counter is 8 ns
    uint duty_val = duty_cycle * wrap_num / 100; // simple percentage proportion

    gpio_set_function(MCLK, GPIO_FUNC_PWM); // set to PWM mode

    uint pwm1_slice_num = pwm_gpio_to_slice_num(MCLK); // PWM is divided in to "slices" this function returns the slice number form GPIO pin value

    pwm_set_wrap(pwm1_slice_num, wrap_num); //only need to set one of them if several PWM pins are in the same slice

    pwm_set_gpio_level(MCLK, duty_val); // this is how you set duty cycle

    pwm_set_output_polarity(pwm1_slice_num, false, false); // if we want to invert pins on the slice set to true

    pwm_set_enabled(pwm1_slice_num, true); // enable the PWM slice


    gpio_init(MDAT);
    gpio_init(MUX_A0);
    gpio_init(MUX_A1);
    gpio_init(MUX_A2);

    gpio_set_dir(MDAT, GPIO_OUT);
    gpio_set_dir(MUX_A0, GPIO_OUT);
    gpio_set_dir(MUX_A1, GPIO_OUT);
    gpio_set_dir(MUX_A2, GPIO_OUT);
    
    gpio_put(MDAT, true);
    gpio_put(MUX_A0, true);
    gpio_put(MUX_A1, true);
    gpio_put(MUX_A2, true);


    const uint led_pin = 25;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    // Initialize chosen serial port
    stdio_init_all();

    gpio_put(MUX_A0, false);
    gpio_put(MUX_A1, false);
    gpio_put(MUX_A2, false);



    // Loop forever
    while (true) {

        // Blink LED
        // printf("Blinking!\r\n");
        gpio_put(led_pin, true);

        sleep_ms(2000);

        gpio_put(led_pin, false);

        sleep_ms(2000);

        // gpio_put(MUX_A0, false);
        // gpio_put(MUX_A1, false);
        // gpio_put(MUX_A2, false);

        // sleep_ms(2000);

        // gpio_put(led_pin, false);

        // gpio_put(MUX_A0, true);
        // gpio_put(MUX_A1, false);
        // gpio_put(MUX_A2, false);

        // sleep_ms(2000);

        // gpio_put(led_pin, true);

        // gpio_put(MUX_A0, false);
        // gpio_put(MUX_A1, true);
        // gpio_put(MUX_A2, false);

        // sleep_ms(2000);

        // gpio_put(led_pin, false);

        // gpio_put(MUX_A0, true);
        // gpio_put(MUX_A1, true);
        // gpio_put(MUX_A2, false);

        // sleep_ms(2000);

        // gpio_put(led_pin, true);

        // gpio_put(MUX_A0, false);
        // gpio_put(MUX_A1, false);
        // gpio_put(MUX_A2, true);

        // sleep_ms(2000);
        
    }
}