/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */


#include "sao.h"
#include "eeprom.h"
#include "settings.h"
#include "stdio.h"
#include "string.h"

const char valid_badge_name[] = "BSV25";
const uint32_t valid_badge_name_len = 5;

const sao_id_t sao_valid_drivers[] = {
		{ .name="CREW",	 .id=1 },
		{ .name="SPEAKER", .id=2 },
		{ .name="SPONSOR", .id=3 },
};
const uint8_t sao_valid_drivers_count = sizeof(sao_valid_drivers)/sizeof(sao_id_t);

uint8_t sao_active_driver_id = 0;

sao_status_t sao_get_header(sao_header_t *header) {
	if (header == 0) {
		return SAO_ERROR;
	}
	sao_status_t status = sao_read(0, (char *) header, sizeof(sao_header_t));
	if (status != SAO_OK) {
		return status;
	}
	if (header->magic != 0x4546494c) {
		return SAO_INVALID;
	}
	return SAO_OK;
}

sao_status_t sao_read(const uint32_t offset, char *buf, uint32_t buflen) {
	HAL_StatusTypeDef status = eeprom_read(offset, (uint8_t *) buf, buflen);
	if (status != HAL_OK) {
		return SAO_ERROR;
	}
	return SAO_OK;
}

sao_status_t sao_write(const uint32_t offset, char * buf, uint32_t buflen) {
	HAL_StatusTypeDef status = eeprom_write(0, (uint8_t *) buf, buflen);
	if (status != HAL_OK) {
		return SAO_ERROR;
	}
	return SAO_OK;
}

// returns:
//			id of a valid driver
//			0 if not found
uint8_t sao_find_valid_driver(const char *driver_name, uint32_t driver_name_len) {
	sao_id_t driver;
	for (uint8_t driver_idx=0; driver_idx<sao_valid_drivers_count; driver_idx++) {
		driver = sao_valid_drivers[driver_idx];
		if (strlen(driver.name) != driver_name_len) {
			continue;
		}
		if (strncmp(driver.name, driver_name, driver_name_len) == 0) {
			return driver.id;
		}
	}
	return 0;
}

uint8_t sao_get_driver_id() {
	sao_header_t header;
	sao_status_t status = sao_get_header(&header);
	if (status != SAO_OK) {
		return 0;
	}

	if (valid_badge_name_len != header.badge_name_len) {
		return 0;
	}
	const uint32_t sao_header_size = sizeof(header);
	char badge_name[header.badge_name_len];
	status = sao_read(sao_header_size, badge_name, header.badge_name_len);
	if (status != SAO_OK) {
		return 0;
	}
	if (strncmp(valid_badge_name, badge_name, header.badge_name_len) != 0) {
		return 0;
	}

	char driver_name[header.driver_name_len+1];
	driver_name[header.driver_name_len]=0;
	status = sao_read(sao_header_size+header.badge_name_len, driver_name, header.driver_name_len);
	if (status != SAO_OK) {
		return 0;
	}
	uint8_t id = sao_find_valid_driver(driver_name, header.driver_name_len);
	return id;
}

void sao_activate() {
	sao_active_driver_id = sao_get_driver_id();
	switch (sao_active_driver_id) {
	case 0:
		//SAO not found or driver not valid
		printf("Valid SAO not found.\r\n");
		return;
	case 1:
		if (settings.crew == 1) {
			return;
		}
		settings.crew=1;
		break;
	case 2:
		if (settings.speaker == 1) {
			return;
		}
		settings.speaker=1;
		break;
	case 3:
		if (settings.sponsor == 1) {
			return;
		}
		settings.sponsor=1;
		break;
	default:
		printf("BUG, adjust SAO activation!\r\n");
		return;
	}
	printf("Activating SAO %s\r\n", sao_valid_drivers[sao_active_driver_id-1].name);
	settings_save();
}

