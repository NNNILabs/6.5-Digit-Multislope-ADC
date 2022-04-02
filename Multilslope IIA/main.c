#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ms.pio.h"

int main()
{
    set_sys_clock_khz(100000, true); //100Mhz main for testing, 96MHz for real thing

    static const uint ledPin = 0;
    static const uint PWMA = 20;
    static const uint PWMB = 21;
    static const uint COMP = 19;
    static const uint MEAS = 17;

    uint32_t pushVar = 1000;
    uint32_t counts = 0;
    uint32_t postCounts = 0;

    PIO pio = pio0;

    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &ms_program);

    gpio_init(ledPin);
    gpio_set_dir(ledPin, GPIO_OUT);

    float div = 10; //100MHz main and 10MHz PIO for testing, div = 10 for real thing
    /*
    Multislope timings
    Processor clock     : 96Mhz
    PIO clock           : 9.6MHz
    PWM frequency       : 300kHz
    Clock per PWM       : 32 cycles
    Duty cycle          : 1/16, 15/16
    PLC         @50Hz   : 6000 PWM cycles
                @60Hz   : 5000 PWM cycles
    Pre-rundown         : 32 clock cycles
    */
    
    ms_program_init(pio, sm, offset, PWMA, MEAS, COMP, div);

    while (true)
    {
        pio_sm_put_blocking(pio, sm, pushVar);
        pio_sm_set_enabled(pio, sm, true);
        gpio_put(ledPin, true);
        counts = ~pio_sm_get_blocking(pio, sm);
        postCounts = ~pio_sm_get_blocking(pio, sm);
        gpio_put(ledPin, false);
        pio_sm_set_enabled(pio, sm, false);
        sleep_ms(1000);
    }
}