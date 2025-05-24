/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#ifndef INC_SAO_H_
#define INC_SAO_H_

#include "stdint.h"

#define SAO_NAME_MAXLEN 20

extern uint8_t sao_active_driver_id;

typedef enum
{
  SAO_OK      		= 0x00U,
  SAO_INVALID		= 0x01U,
  SAO_ERROR			= 0x02U,
} sao_status_t;

typedef struct {
	uint32_t magic;
	uint8_t badge_name_len;
	uint8_t driver_name_len;
	uint8_t driver_data_len;
	uint8_t num_extra_drivers;
} sao_header_t;

typedef struct {
	char name[SAO_NAME_MAXLEN];
	uint8_t id;
} sao_id_t;

sao_status_t sao_get_header(sao_header_t *header);
sao_status_t sao_read(const uint32_t offset, char *buf, uint32_t buflen);
sao_status_t sao_write(const uint32_t offset, char *buf, uint32_t buflen);
uint8_t sao_get_driver_id();
void sao_activate();

#endif /* INC_SAO_H_ */
