/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#include "stm32l0xx_hal.h"
#include "settings.h"

settings_t settings;

void settings_load() {
	settings.raw = *((uint8_t *) SETTINGS_ADDRESS);
}

void settings_save() {
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAMDATA_BYTE, SETTINGS_ADDRESS, settings.raw);
	HAL_FLASH_Lock();
}
