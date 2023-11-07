/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LORAWAN__H
#define APP_LORAWAN__H

#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include <zephyr/random/rand32.h>
#include <zephyr/lorawan/lorawan.h>

#include <time.h>

/* customize based on network configuration */
/* put this parameters in TTN application */

#define LORAWAN_DEV_EUI			{ 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x21, 0xA5 }
#define LORAWAN_JOIN_EUI		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define LORAWAN_APP_KEY			{ 0xC7, 0x32, 0x0F, 0x37, 0xFF, 0x62, 0xE0, 0xA8, 0x4E, 0x94, 0xC1, 0x9C, 0x27, 0x2B, 0xFA, 0x4C }

#endif /* APP_LORAWAN__H */