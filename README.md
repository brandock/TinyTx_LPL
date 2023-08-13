# TinyTx_LPL
TinyTx firmware by Nathan Chantrell, modified to work with the OpenEnergyMonitor RFM69_LPL radio library.

# Background
In 2015 when I started working with the OpenEnergyMonitor ecosystem, Nathan Chantrell's TinyTx helped me get started creating my own sensors. I did not know him, but I know I am not alone in appreciating his contribution to the OpenEnergyMonitor community. It is my understanding that Nathan died in 2020. This repository is designed to honor his contributions, and bring his original firmware sketches for ATtiny84-based TinyTx boards up to date with the recent change in the OpenEnergyMonitor stack to use the RFM69 radio library from Felix Rusu at LowPowerLab. 

Another repository takes a slightly different approach to continuing Nathan's work: https://github.com/amadeuspzs/TinyTX. If you are interested in using the RFM69HW radios this would be a good place to go. If you are want to stay compatible with the OpenEnergyMonitor stack, which uses RFM12B/RFM69CW radios, this repository is intended for that purpose.

# Resources
Nathan's website via Archive.org: https://web.archive.org/web/20150202220320/http://nathan.chantrell.net/tinytx-wireless-sensor/

Nathan's TinyTX repo on Github: https://github.com/nathanchantrell/TinyTX

Archive of TinyTX3 hardware: https://github.com/amadeuspzs/TinyTX/blob/main/TinyTX3

# Using RFM69_LPL
Trystan Lea and the OpenEnergyMonitor community created a cut-down version of the full RFM69 library designed for use with AVR-DB, ATmega328, and ATtiny84. This radio library is used in place of JeeLib. (Note that JeeLib is still used in OpenEnergyMonitor firmware for the low-power sleep functions.)

https://github.com/openenergymonitor/RFM69_LPL

# Using Spence Konde ATTinyCore for Arduino IDE
OpenEnergyMonitor uses Spence Konde's cores to compile RFM69_LPL firmware. DxCore is used for AVR-DB. ATTinyCore is used for ATtiny84. ATTinyCore is an excellent, modern board manager for ATtiny84. It is a major improvement over the arduino-tiny core that was typical when TinyTx was created.

# Using RFM69CW Radios
The RFM69CW is a newer radio that is pin-compatible with the RFM12B footprint used by TinyTx and all OpenEnergyMonitor hardware. This radio was already more common than the RFM12B when I started working with TinyTx in 2015. I think I might have had one actual RFM12B at some point, but I am not sure. In any case, the RFM69CW radio is the one I have used for testing the firmware.

# Original Sketches and Modified Sketches
The original sketches will be posted with _Nathan_Chantrell_Original appended. The modified sketches will be posted with _LPL tacked on. The goal of the updates is to be minimally invasive and leave as much of the original sketch as possible unchanged.

![TinyTX3_Front_Back_s](https://github.com/brandock/TinyTx_LPL/assets/17953028/662c0414-b3f3-4194-8896-3826b442ee06)
