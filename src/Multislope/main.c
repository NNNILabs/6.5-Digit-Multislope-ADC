#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

volatile uint multiSlopeFreq = 200000; // clock frequency for multislope
volatile uint duty_cycle = 50;
uint nsec_period = 0;
uint wrap_num = 0;
uint duty_val = 0;

const uint8_t MDAT = 17; // SPI MISO Pin
const uint8_t MCLK = 18; // SPI Clock Pin
const uint8_t M_EN = 16;

const u_int8_t MUX_A0 = 22;
const u_int8_t MUX_A1 = 26;
const u_int8_t MUX_A2 = 27;

const u_int8_t PWMA = 20; // need to be in the same PWM slice
const u_int8_t PWMB = 21;
const u_int8_t COMP = 19;

void updatePWMVals(){
    nsec_period = 1000000000/multiSlopeFreq; // compute the period in nanoseconds
    wrap_num = nsec_period/8; // (Optimize, by getting the actual frequency and using that for math) since the CPU frequency is 125MHz - one iteration of the PWM counter is 8 ns
    duty_val = duty_cycle * wrap_num / 100; // simple percentage proportion
}

void on_pwm_wrap() {
    if(gpio_get_out_level(COMP)){
        duty_cycle = 80;
    }else{
        duty_cycle = 20;
    }
    updatePWMVals();
    pwm_set_gpio_level(PWMA, duty_val);
    pwm_set_gpio_level(PWMB, duty_val);
}

int main() {
    stdio_init_all();
    updatePWMVals();


    gpio_set_function(PWMA, GPIO_FUNC_PWM); // set to PWM mode
    gpio_set_function(PWMB, GPIO_FUNC_PWM);

    uint pwm1_slice_num = pwm_gpio_to_slice_num(PWMA); // PWM is divided in to "slices" this function returns the slice number form GPIO pin value

    // Mask our slice's IRQ output into the PWM block's single interrupt line,
    // and register our interrupt handler
    pwm_clear_irq(pwm1_slice_num);
    pwm_set_irq_enabled(pwm1_slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);



    pwm_set_wrap(pwm1_slice_num, wrap_num); //only need to set one of them if several PWM pins are in the same slice

    pwm_set_gpio_level(PWMA, duty_val); // this is how you set duty cycle

    pwm_set_output_polarity(pwm1_slice_num, false, true); // if we want to invert pins on the slice set to true

    pwm_set_enabled(pwm1_slice_num, true); // enable the PWM slice


    gpio_init(MDAT);
    gpio_init(MUX_A0);
    gpio_init(MUX_A1);
    gpio_init(MUX_A2);
    gpio_init(COMP);
    gpio_init(MCLK);

    gpio_set_dir(MDAT, GPIO_OUT);
    gpio_set_dir(MCLK, GPIO_OUT);
    gpio_set_dir(MUX_A0, GPIO_OUT);
    gpio_set_dir(MUX_A1, GPIO_OUT);
    gpio_set_dir(MUX_A2, GPIO_OUT);
    gpio_set_dir(COMP, GPIO_IN);
    
    gpio_put(MDAT, true);
    sleep_us(10);
    gpio_put(MCLK, true);
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
        gpio_put(led_pin, true);
        sleep_ms(1000);
        gpio_put(led_pin, false);
        sleep_ms(1000);
    }
}