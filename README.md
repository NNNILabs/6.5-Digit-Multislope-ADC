# 6.5 Digit Multislope ADC
## Analog to digital converter than can resolve voltages down to 10uV
### PCBs for this project were sponsored by JLCPCB: https://jlcpcb.com/RHS

#### About

![ADC](https://github.com/NNNIIndia/6.5-Digit-Multislope-ADC/blob/main/Images/20220113_014830.jpg)

The multislope topology fits comfortably between dual-slope and delta-sigma methods - very few precision components are needed to get very good results. Multislope is a charge balancing method - the input voltage is converted to a current (through the input resistor, one side of which is a virtual ground) and fed continuously into the integrator, and positive and negative reference voltages (which are converted to currents in the same way) are periodically injected into the integrator to maintain net zero charge. This is made possible using a clocked zero crossing comparator which determines which reference is to be switched into the inetgrator every clock cycle. 

This ADC has all the basic parts needed to operate it as a standalone +/-10V digitizer - an LM399 voltage reference, ref amps that convert the reference voltage to +/-10V, an input multiplexer, an input buffer, and the basic multislope components - a resistor network, an analog switch, a composite integrator, a fast comparator and a D latch. Using the on-board jumpers, this board can be configured to run in the "free-running" state (which can be considered a rudimentary delta-sigma method) and in PWM mode, with a constant number of switch transitions of remove errors due to analog switch charge injection.

#### Statistics

-to be added-

#### Errata

- D Latch outputs are reversed if you want to use the ADC in free-running mode, luckily this can be changed easily by crossing over wires on the jumpers
- For the present PWM PIO code, the D latch has to be removed and the MEAS and SW-S3 pins have to be bridged together

