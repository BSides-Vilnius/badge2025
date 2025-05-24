/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#include "stdint.h"

#define GAUGE_VDIV_RATIO	0.766
#define GAUGE_COUNT			2

typedef struct {
	uint16_t vbat;
	uint16_t vref;
} vgauge_t;

void print_power();

#endif /* INC_BATTERY_H_ */
