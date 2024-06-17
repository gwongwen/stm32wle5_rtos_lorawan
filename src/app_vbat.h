/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_VBAT_H
#define APP_VBAT_H

//  ======== includes ==============================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/sensor.h>

//  ======== prototypes ============================================
int8_t app_stm32_vbat_init(const struct device *dev);
uint16_t app_stm32_get_vbat(const struct device *dev);

#endif /* APP_VBAT_H */