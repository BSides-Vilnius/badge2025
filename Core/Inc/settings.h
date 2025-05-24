/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include "stdint.h"

#define SETTINGS_ADDRESS 0x08080000

typedef union {
	uint8_t raw;
	struct {
		uint8_t padd:4;
		uint8_t crew:1;
		uint8_t sponsor:1;
		uint8_t speaker:1;
		uint8_t sparkle:1;
	};
} __attribute__ ((aligned (1), packed)) settings_t;

extern settings_t settings;

void settings_save();
void settings_load();

#endif /* INC_SETTINGS_H_ */
