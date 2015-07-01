# LogicAlNucleo

A SUMP compatible Logical Analyser for the NucleoF401RE (STM32F4xx) up to 10MSPS, 8Ch, 32K samples memory.

This will turn any NucleoF401RE (will work with others but it is not tested) system into a Logical Analyser compatible with a subset of the SUMP protocol. It can be used with clients such as [PulseView](http://sigrok.org/wiki/PulseView), [sigrok-cli](http://sigrok.org/wiki/Sigrok-cli), and [LogicSniffer](http://www.lxtreme.nl/ols/). While it is not as feature complete as other products, such as the [OLS](http://dangerousprototypes.com/docs/Open_Bench_Logic_Sniffer), it can turn that STM32 board that is lying around into a no frills, bare to the bones, logic analyser.

Sampling rate up to 500KSPS should work on most platforms. Higher than that, only the F401RE, or other similar 84Mhz platform should provide results with accurate timing measurements up to 10MSPS (probably near 20MSPS is achievable). If you wish to add support for other platforms, please focus in creating the appropriate unrolled loops. +150Mhz platforms can be easily supported as the more generic approach (wait_ns) can be used. Alternatively an interrupt based approach would be much more precise (but slower).

PORTB is current used, and Pins PB_0 to PB_7 are reported. Unfortunately these pins are scattered over the board and are not contiguous. Check [this](http://developer.mbed.org/platforms/ST-Nucleo-F401RE/) diagram to find them.

This implementation was based in the [mbed](https://mbed.org/) environment in order to increase its compatibility, as new targets are expected to be supported. The focus of this implementation is compatibility and extensibility, and not only performance.

## Features

### Supported
- Configurable sampling rate up to 10Mhz on the F401RE platform
- Basic parallel triggers
- Generic compatibility through the MBED API

### Planned
- RLE support
- Serial triggers
- Post trigger delay
- External and Internal test modes

### Limitations

- The board will frequently freeze and require a reset. This happens because the CPU is not that powerful. Reaching 10MSPS requires the use of synchronous code (instead of a much better asynchronous, Interrupt based approach). If the sampling function is waiting for results or for a trigger, the board will be "stuck". Just push the reset and move on as nothing is wrong.

## Screenshots
Just to prove it works and because screenshots are always nice.

Channel 4 with a signal permanently set to 1, others set to 0.
This capture was made using the test mode active. In this mode the board will provide a PWM signal in D9, with a period of 50us and pulse width of 20us (30us off). This pin was connected to D4 (PB_3) and sampled with great accuracy at 1Mhz.


![alt text](https://raw.githubusercontent.com/jpbarraca/LogicalNucleo/master/screenshots/screen1.png "LogicSniffer in Test Mode")


## Download

Get the source code from github and compile it using the ARM toolchain available from the [Launchpad page](https://launchpad.net/gcc-arm-embedded/+download). If you have the toolchain available, the makefile will take care of all compilation and linking.

In alternative you can also use the code in [mbed](https://mbed.org/). 

Finally download the latest precompiled binary from the [releases web page](https://github.com/jpbarraca/LogicAlNucleo/releases) and burn it. The binary file suits the NUCLEO-F401RE board.