# LogicAlNucleo

A SUMP compatible Logical Analyser for the NucleoF401RE (STM32F4xx) almost up to 10MSPS, 8Ch, 32K samples memory.

This will turn any NucleoF401RE (will work with other boards but it was not tested) system into a Logical Analyser compatible with a subset of the SUMP protocol. It can be used with clients such as [PulseView](http://sigrok.org/wiki/PulseView), [sigrok-cli](http://sigrok.org/wiki/Sigrok-cli), and [LogicSniffer](http://www.lxtreme.nl/ols/). While it is not as feature complete as other products, such as the [OLS](http://dangerousprototypes.com/docs/Open_Bench_Logic_Sniffer), it can turn that STM32 board that is lying around into a no frills, bare to the bones, logic analyser.

Sampling rate up to 500KSPS should work on most platforms. Higher than that, only the F401RE, or other similar >84Mhz platform should provide results with accurate timing measurements up to 5MSPS. 10MSPS will be accepted but do not trust timing information as the board cannot really keep up with this speed. However this sampling rate can still be usefull. If you wish to add support for other platforms, please focus in creating the appropriate unrolled loops callibrated with an osciloscope.

PORTB is current used, and Pins PB_0 to PB_7 are reported. Unfortunately these pins are scattered over the board and are not contiguous. Check [this](http://developer.mbed.org/platforms/ST-Nucleo-F401RE/) diagram to find them.

This implementation was based in the [mbed](https://mbed.org/) environment in order to increase its compatibility, as new targets are expected to be supported. The focus of this implementation is compatibility and extensibility, and not only performance.

## Features

### Supported
- Configurable sampling rate up to 10Mhz on the F401RE platform
- Basic parallel triggers
- Generic compatibility with other platforms through the MBED API
- Test mode where PWM signals from 1us to 500ms will be generated and then captured. You can use this mode to test the accuracy of each mode.

### Planned
- RLE support
- Serial triggers (for speeds lower than 1MSPS)
- Post trigger delay
- External test modes

### Limitations

- Reaching 10MSPS requires the use of synchronous code (instead of a much better asynchronous, interrupt based approach, yet slower). If the sampling function is waiting for results or for a trigger, the board will be "stuck" waiting. Just push the reset and move on as nothing is wrong. The Green LED will be active if the board is waiting for the acquisition process to finish.

## Screenshots
Just to prove it works and because screenshots are always nice.

![alt text](https://raw.githubusercontent.com/jpbarraca/LogicalNucleo/master/screenshots/screen1.png "LogicSniffer in Test Mode")


## Download

Get the source code from github and compile it using the ARM toolchain available from the [Launchpad page](https://launchpad.net/gcc-arm-embedded/+download). If you have the toolchain available, the Makefile will take care of all compilation and linking.

In alternative you can also use the code in [mbed](https://mbed.org/). Just load the code without the mbed directory. 

Finally download the latest precompiled binary from the [releases web page](https://github.com/jpbarraca/LogicAlNucleo/releases) and flash it. The binary file suits the NUCLEO-F401RE board.