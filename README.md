# Code for 6sens Omnitilt Project : test LoRaWAN sample application

## Overview
This simple application allows us to demonstrate and validate the LoRaWAN subsystem Zephyr.
This application connects to The Things Network (TTN) community (LoRaWAN network server) and sends a string (Hello World!).

LoRaWAN Device EUI, Join EUI and Application Key must be entered into the app_lorawan.h header file prior to compiling.

We have to create an application and add end device to the TTN network to see if the code is well. The parameters of the end device are defined in app_lorawan.h header file.

If specification 1.0.4 or 1.1.1 of Lorawan network-layer is used, the code stores the DevNonce in NVS (Non-volatile Storage) as per LoRaWAN 1.0.4 Specifications.

The LoRaWAN network-layer setting are:

    - frequency plan                    Europe 863-870 MHz (SF for RX2)
    - LoRaWAN version                   LoRWAN speccification 1.0.4
    - regional parameters version       RP002 regional parameters 1.0.4
    - LoRaWAN class                     Class A

**First Board used** : Cicecrone board by Move-X

**Second Board used** : Original MAMWLExx board, powered by battery/solar panel. (see 6sens_omnitilt repository/hardware part, for more information).

## Building and Running
Before building the sample, make sure to select the correct region in the prj.conf file.

The following commands clean build folder, build and flash the sample:

**Command to use**

west build -t pristine

west build -p always -b stm32wl_dw1000_iot_board applications/stm32wle5_rtos_lora

west flash --runner stm32cubeprogrammer