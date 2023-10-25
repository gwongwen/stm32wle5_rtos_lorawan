/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_nvs.h"

int app_nvs_init(struct nvs_fs *fs)
{
	struct flash_pages_info info;
	int ret = 0;

	fs->flash_device = NVS_PARTITION_DEVICE;
	if (!device_is_ready(fs->flash_device)) {
		printk("flash device %s is not ready\n", fs->flash_device->name);
		return 0;
	}

	fs->offset = NVS_PARTITION_OFFSET;
	ret = flash_get_page_info_by_offs(fs->flash_device, fs->offset, &info);
	if (ret) {
		printk("unable to get page info. error: %d\n", ret);
		return 0;
	}

	fs->sector_size = info.size;
	if (!fs->sector_size || fs->sector_size % info.size) {
		printk("invalid sector size");
		return -EINVAL;
	}

	fs->sector_count = 3U;

	ret = nvs_mount(fs);
	if (ret) {
		printk("flash init failed. error: %d\n", ret);
		return 0;
	}

#ifdef NVS_CLEAR
	ret = nvs_clear(fs);
	if (ret) {
		printk("flash clear failed. error: %d\n", ret);
		return;
	} else {
		printk("cleared NVS from flash\n");
	}
#endif
}

int app_nvs_init_param(struct nvs_fs *fs, uint16_t id, void *data)
{
	int ret = 0;

	ret = nvs_read(fs, id, data, sizeof(data));
	if (ret > 0) {
		printk("ID: %d, address: %s", id, data);
	} else {
		printk("no address found, adding %s at id %d\n", data, id);
		(void)nvs_write(fs, id, data, sizeof(data));	
	}
	return 0;
}