/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_lorawan.h"
#include "app_nvs.h"

//  ======== defines ============================================
#define 	DELAY 			K_MINUTES(5)
#define 	MAX_DATA_LEN	10
#define 	OTAA
//#define		ABP

//  ======== globals ============================================
char data_tx[MAX_DATA_LEN] = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd'};

// downlink callback
static void dl_callback(uint8_t port, bool data_pending, int16_t rssi, int8_t snr, uint8_t len, const uint8_t *data)
{
	printk("downlink data received: ");
    for(int8_t i=0; i < len; i++) {
		printk("%02X ", data[i]);
	}
    printk("\n");
	printk("port: %d, pending: %d, RSSI: %ddB, SNR: %ddBm\n", port, data_pending, rssi, snr);
}

// ADR change callback
static void lorwan_datarate_changed(enum lorawan_datarate dr)
{
	uint8_t unused, max_size;

	lorawan_get_payload_sizes(&unused, &max_size);
	printk("new datarate: DR_%d, max payload: %d\n", dr, max_size);
}

static const struct gpio_dt_spec led_tx = GPIO_DT_SPEC_GET(LED_TX, gpios);
static const struct gpio_dt_spec led_rx = GPIO_DT_SPEC_GET(LED_RX, gpios);

//  ======== main ===============================================
int8_t main(void)
{
	const struct device *lora_dev;
	static struct nvs_fs fs;
	
	struct lorawan_join_config join_cfg;
	uint16_t dev_nonce = 0u;

	const struct device *bat_dev;
	uint16_t vbat;
	uint32_t max_cnt = 0;

#ifdef OTAA
	uint8_t dev_eui[] 	= LORAWAN_DEV_EUI;
	uint8_t join_eui[]	= LORAWAN_JOIN_EUI;
	uint8_t app_key[]	= LORAWAN_APP_KEY;
#endif

#ifdef ABP
    uint8_t dev_addr[] = LORAWAN_DEV_ADDR;
    uint8_t nwk_skey[] = LORAWAN_NWK_SKEY;
    uint8_t app_skey[] = LORAWAN_APP_SKEY;
    uint8_t app_eui[]  = LORAWAN_APP_EUI;
#endif

	uint32_t gps_time;
	time_t unix_time;
	struct tm timeinfo;
	char buf[32];

	int ret = 0;
	ssize_t err = 0;
	int8_t itr = 1;

	// setup tx led at GPIO PC0
	ret = gpio_pin_configure_dt(&led_tx, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	// setup rx led at GPIO PC1
	ret = gpio_pin_configure_dt(&led_rx, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	printk("Zephyr LoRaWAN Node Example. Board: %s\n", CONFIG_BOARD);

	// initialization and reading/writing the devnonce parameter
	app_nvs_init(&fs);
	app_nvs_init_param(&fs, NVS_DEVNONCE_ID, &dev_nonce);
	
	printk("starting Loarawan node\n");
	lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	if (!device_is_ready(lora_dev)) {
		printk("%s: device not ready\n", lora_dev->name);
		return 0;
	}

	printk("starting Lorawan stack\n");
	ret = lorawan_set_region(LORAWAN_REGION_EU868);
	ret = lorawan_start();
	if (ret < 0) {
		printk("lorawan_start failed. error: %d\n", ret);
		return 0;
	}

	// enable ADR
    lorawan_enable_adr(true);

	// enable callbacks
	struct lorawan_downlink_cb downlink_cb = {
		.port = LW_RECV_PORT_ANY,
		.cb = dl_callback
	};
	lorawan_register_downlink_callback(&downlink_cb);
	lorawan_register_dr_changed_callback(lorwan_datarate_changed);

#ifdef OTAA
	join_cfg.mode = LORAWAN_ACT_OTAA;
	join_cfg.dev_eui = dev_eui;
	join_cfg.otaa.join_eui = join_eui;
	join_cfg.otaa.app_key = app_key;
	join_cfg.otaa.nwk_key = app_key;
	join_cfg.otaa.dev_nonce = dev_nonce;
#endif

#ifdef ABP
    join_cfg.mode = LORAWAN_ACT_ABP;
    join_cfg.dev_eui = dev_addr;
    join_cfg.abp.dev_addr = dev_addr;
    join_cfg.abp.app_skey = app_skey;
    join_cfg.abp.nwk_skey = nwk_skey;
    join_cfg.abp.app_eui  = app_eui;
#endif


#ifdef OTAA
	do {
		printk("joining network using OTAA, dev nonce %d, attempt %d\n", join_cfg.otaa.dev_nonce, itr++);
		ret = lorawan_join(&join_cfg);
		ret = gpio_pin_toggle_dt(&led_rx);
			if (ret < 0) {
				return 0;
			}

		if (ret < 0) {
			if ((ret =-ETIMEDOUT)) {
				printk("timed-out waiting for response.\n");
			} else {
				printk("join network failed. error: %d\n", ret);
			}
		} else {
			printk("OTAA join successful\n");
		}

		dev_nonce++;
		join_cfg.otaa.dev_nonce = dev_nonce;

		// save value away in Non-Volatile Storage.
		err = nvs_write(&fs, NVS_DEVNONCE_ID, &dev_nonce, sizeof(dev_nonce));
		if (err < 0) {
			printk("NVS: failed to write id %d. error: %d\n", NVS_DEVNONCE_ID, err);
		}

		if (ret < 0) {
			// If failed, wait before re-trying.
			k_sleep(DELAY);
		}
	} while (ret != 0);	
#endif

#ifdef ABP
	do {
		ret = lorawan_join(&join_cfg);
		ret = gpio_pin_toggle_dt(&led_rx);
			if (ret < 0) {
				return 0;
			}

		if (ret < 0) {
			printk("join network failed. error: %d\n", ret);
			k_sleep(DELAY);
		} else {
			printk("ABP join successful\n");
		}
	} while (ret != 0)	;
#endif

#ifdef CONFIG_LORAWAN_APP_CLOCK_SYNC
	lorawan_clock_sync_run();
	ret = lorawan_clock_sync_get(&gps_time);
		if (ret != 0) { 
			printk("lorawan clock sync get failed. error: %d\n", ret);
		} else {
			/* 
			 * The difference in time between UNIX (epoch Jan 1st 1970) and
			 * GPS (epoch Jan 6th 1980) is 315964800 seconds. This is a bit
			 * of a fudge as it doesn't take into account leap seconds and 
			 * hence is out by roughly 18 seconds. 
			 *
			 */
			unix_time = gps_time - 315964800;
			localtime_r(&unix_time, &timeinfo);
			strftime(buf, sizeof(buf), "%A %B %d %Y %I:%M:%S %p %Z", &timeinfo);
			printk("GPS time (seconds since Jan 6th 1980) = %"PRIu32", UTC time: %s\n", gps_time, buf);
		}
#endif

	// initialization of ADC device
	app_stm32_vbat_init(dev);

	printk("sending data...\n");
//	for (itr = 0; itr < 10 ; itr++) {
	while (1) {
		ret = lorawan_send(2, data_tx, sizeof(data_tx), LORAWAN_MSG_CONFIRMED);

		if (ret == -EAGAIN) {
			printk("lorawan_send failed: %d. continuing...\n", ret);
			k_sleep(DELAY);
			continue;
		} else if (ret < 0) {
			printk("lorawan_send failed. error: %d\n", ret);
			return 0;
		}
		
		ret = gpio_pin_toggle_dt(&led_tx);
			if (ret < 0) {
				return 0;
			}
		printk("data sent !\n");

		vbat = app_stm32_get_vbat(dev);
		// writing data in the first page of 2kbytes
		(void)nvs_write(&fs, NVS_BAT_ID, &vbat, sizeof(vbat));
		
		max_cnt++;
		// writing data in the first page of 2kbytes
		(void)nvs_write(&fs, NVS_SENSOR_ID, &max_cnt, sizeof(max_cnt));
		k_sleep(DELAY);
	}
	// reading the first page
	ret = nvs_read(&fs, NVS_SENSOR_ID, &max_cnt, sizeof(max_cnt));
	// printing data stored in memory
	printk("max value of counter: %"PRIu32"\n",max_cnt);

	// reading the first page
	ret = nvs_read(&fs, NVS_BAT_ID, &vbat, sizeof(vbat));
	// printing data stored in memory
	printk("min value of battery: %"PRIu32"\n",vbat);
	return 0;
}