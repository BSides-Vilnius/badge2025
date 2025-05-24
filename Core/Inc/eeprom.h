/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

#include "stdint.h"
#include "stm32l0xx_hal.h"

#define EEPROM_ADDR(addr) ((0x50|((addr)&7))<<1)
#define EEPROM_PAGE_SIZE 8

HAL_StatusTypeDef eeprom_read(const uint16_t addr, uint8_t * buf, const uint32_t buflen);
HAL_StatusTypeDef eeprom_write(const uint16_t addr, uint8_t * buf, const uint32_t buflen);

#endif /* INC_EEPROM_H_ */
