#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ms.pio.h"

int main()
{
    set_sys_clock_khz(100000, true);

    static const uint ledPin = 0;
    static const uint PWMA = 20;
    static const uint PWMB = 21;
    static const uint COMP = 19;
    static const uint MEAS = 17;

    PIO pio = pio0;

    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &ms_program);

    gpio_init(ledPin);
    gpio_set_dir(ledPin, GPIO_OUT);

    float div = 10;//(float)clock_get_hz(clk_sys) / pioFrequency;
    /*
    Multislope timings
    Processor clock     : 96Mhz
    PIO clock           : 4.8MHz
    PWM frequency       : 300kHz
    Clock per PWM       : 16 cycles
    Duty cycle          : 2/16, 14/16
    PLC         @50Hz   : 6000 PWM cycles
                @60Hz   : 5000 PWM cycles
    Pre-rundown         : 32 clock cycles
    */
    
    ms_program_init(pio, sm, offset, PWMA, MEAS, COMP, div);

    pio_sm_set_enabled(pio, sm, true);

    while (true)
    {
        pio_sm_put(pio, sm, 1);
        gpio_put(ledPin, 1);
        sleep_ms(1000);
        pio_sm_put(pio, sm, 0);
        gpio_put(ledPin, 0);
        sleep_ms(1000);
    }
}