/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#include "battery.h"
#include "main.h"
#include "adc.h"
#include <stdio.h>

void print_power() {
	vgauge_t gauge;
	uint32_t tick = HAL_GetTick();
	HAL_GPIO_WritePin(GAUGE_DISABLED_GPIO_Port, GAUGE_DISABLED_Pin, RESET);
	HAL_ADC_Start_DMA(&hadc, (uint32_t *) &gauge, GAUGE_COUNT);
	HAL_GPIO_WritePin(GAUGE_DISABLED_GPIO_Port, GAUGE_DISABLED_Pin, SET);
	HAL_DMA_PollForTransfer(hadc.DMA_Handle, HAL_DMA_FULL_TRANSFER, 100);
	float voltage = 3.3 * gauge.vbat / (4095 * GAUGE_VDIV_RATIO);
	printf("uptime: %lu, voltage: %.3f (val:%u, ref:%u)\r\n", tick, voltage, gauge.vbat, gauge.vref);
}



