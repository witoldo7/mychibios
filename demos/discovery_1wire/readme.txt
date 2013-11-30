*****************************************************************************
** ChibiOS/RT port for ARM-Cortex-M4 STM32F407.                            **
*****************************************************************************

** TARGET **

The demo runs on an ST STM32F4-Discovery board with one or several one wire
sensors connected : 

a one wire componant need vdd, vss and data, in this demo data (DQ) of all
sensors should be connected on GPIOD PIN 8.

a 4,7Kohms pullup resistor should be connected between vdd (3.3V) and data bus
(GPIOD PIN 8).

output is done on a tty console on serial over usb : wire micro usb port of
discovery on your computer, then open hyperterminal to connect to it.

under linux, screen /dev/ttyACM0 does the job

** The Demo **


The demo show how to use a simple library for accessing one wire devices.

The library is made of files oneWire.h and oneWire.c
ds1820b.h and ds1820b.c are provided as example to show how to extend library
for specific one wire devices.

This is a library for OneWire Master bus http://en.wikipedia.org/wiki/1-Wire
based on advices found on dallas documentation : 
http://www.maximintegrated.com/app-notes/index.mvp/id/214
this library use an usart to avoid to consume cpu cycle in bit banging
the library is synchronous, but when waiting it yield the processor to chibios.

the usart is configured in half duplex mode, so there is only TX to wire,
RX is connected internally for data reception when the bus is in reception mode
 

The demo lanch a basic shell and wait for commands :

mem : memory info
info : chibios info

ow (for one wire) =>

ow search : print the unique address of all the device connected on the bus
ow rom : print address of one device *ONLY* if there is only one device on the bus
ow t : get value of all the temperature sensors maxim 18b20 on the bus one
       each after other.
ow dt : ask all the  18b20 sensors to initiate acquisition, wait for the acquisition
       to be done, then get all the values, better way whan there is more than 1 sensors

** Build Procedure **

The demo has been tested by using the free Codesourcery GCC-based toolchain. 
just modify the TRGT line in the makefile in order to use
different GCC toolchains.

** Notes **

Some files used by the demo are not part of ChibiOS/RT but are copyright of
ST Microelectronics and are licensed under a different license.
Also note that not all the files present in the ST library are distributed
with ChibiOS/RT, you can find the whole library on the ST web site:

                             http://www.st.com




Alexandre Bustico
alexandre.bustico@enac.fr
