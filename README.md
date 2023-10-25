# Code for 6sens Omnitilt Project : test LoRaWAN sample application

## Overview
This simple application allows us to demonstrate and validate the LoRaWAN subsystem Zephyr.
This application just sends a string (Hello World!) to The Things Network (TTN) community (LoRaWAN network server).

We have to create an application and add end device to the TTN network to see if the code is well. The parameters of the end device are defined in app_lorawan header file.

The LoRaWAN network-layer setting are:

    - frequency plan                    Europe 863-870 MHz (SF for RX2)
    - LoRaWAN version                   LoRWAN speccification 1.0.3
    - regional parameters version       RP001 rRegional Parameters 1.0.3 revision A
    - LoRaWAN class                     Class A

**First Board used** : Cicecrone board by Move-X

**Second Board used** : Original MAMWLExx board, powered by battery/solar panel. (see 6sens_omnitilt repository/hardware part, for more information)

## Building and Running
Before building the sample, make sure to select the correct region in the prj.conf file.

The following commands clean build folder, build and flash the sample:

**Command to use**

west build -t pristine

west build -p always -b stm32wl_dw1000_iot_board applications/stm32wle5_rtos_lora

west flash --runner stm32cubeprogrammer