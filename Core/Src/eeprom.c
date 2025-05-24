/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#include "eeprom.h"
#include "i2c.h"
#include "string.h"

HAL_StatusTypeDef eeprom_read(const uint16_t addr, uint8_t * buf, const uint32_t buflen) {
	HAL_StatusTypeDef state = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR((addr>>8)), addr&0xff, 1, buf, buflen, 10);
	if (state == HAL_ERROR && hi2c1.ErrorCode == HAL_I2C_ERROR_TIMEOUT) {
		// recover if hardware read failed
		__HAL_I2C_DISABLE(&hi2c1);
		__HAL_I2C_ENABLE(&hi2c1);
	}
	return state;
}

HAL_StatusTypeDef eeprom_write(const uint16_t addr, uint8_t * buf, const uint32_t buflen) {
	uint8_t dev_addr; //dev addr is 3 bits only, other bits include 1010 control code and a R/W bit
	uint8_t mem_addr; //mem addr is 8 bits
	uint32_t write_len;
	HAL_StatusTypeDef state=HAL_OK;
	for (int offset=0; offset<buflen; offset+=EEPROM_PAGE_SIZE) {
		dev_addr=EEPROM_ADDR(((addr+offset)>>8));
		mem_addr=(addr+offset)&0xff;
		write_len = buflen - offset;
		if (write_len > EEPROM_PAGE_SIZE) {
			write_len = EEPROM_PAGE_SIZE;
		}
		state = HAL_I2C_IsDeviceReady(&hi2c1, dev_addr, 100, 10);
		if (state == HAL_OK) {
			state = HAL_I2C_Mem_Write(&hi2c1, dev_addr, mem_addr, 1, buf+offset, write_len, 10);
		}
		if (state != HAL_OK) {
			break;
		}
	}
	if (state == HAL_ERROR && hi2c1.ErrorCode == HAL_I2C_ERROR_TIMEOUT) {
		// recover if hardware write failed
		__HAL_I2C_DISABLE(&hi2c1);
		__HAL_I2C_ENABLE(&hi2c1);
	}
	return state;
}
