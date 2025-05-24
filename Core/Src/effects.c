/**
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  */

#include "effects.h"
#include "stdlib.h"
#include "stdio.h"

uint8_t effect_state;

int min(int a, int b) {
	return (a > b ? b : a);
}

int max(int a, int b) {
	return (a > b ? a : b);
}


/******************
 * SIMPLE PULSING *
 ******************/
#define PULSE_INTENSITY_LOW	6
#define MASK_LEVEL	0x000000FF
#define MASK_BLUE	0x0000FF00
#define MASK_GREEN	0x00FF0000
#define MASK_RED 	0xFF000000

static void effect_pulse(sk9822_t *leds, const uint8_t led_count, const uint32_t color_mask, const uint8_t level) {
	  uint8_t pixel;
	  effect_state = (effect_state+1) % led_count;
	  for (int led_idx=0; led_idx<led_count; led_idx++) {
		  pixel = PULSE_INTENSITY_LOW + abs(effect_state-19)*2;
		  leds[led_idx] = (sk9822_t){.r = pixel, .g = pixel, .b= pixel, .level=level, .begin=7};
		  leds[led_idx].raw &= color_mask;
	  }

}

void effect_pulse_red(sk9822_t *leds, const uint8_t led_count, const uint8_t level) {
	effect_pulse(leds, led_count, MASK_LEVEL|MASK_RED, level);
}

void effect_pulse_green(sk9822_t *leds, const uint8_t led_count, const uint8_t level) {
	effect_pulse(leds, led_count, MASK_LEVEL|MASK_GREEN, level);
}

void effect_pulse_blue(sk9822_t *leds, const uint8_t led_count, const uint8_t level) {
	effect_pulse(leds, led_count, MASK_LEVEL|MASK_BLUE, level);
}



/******************
 * CIRCULAR WAVES *
 ******************/
#define WAVE_MAP_ROWS 18
#define WAVE_MAP_COLS 3
#define WAVE_LENGTH 36
#define WAVE_OFFSET 6
const uint8_t wave_map[WAVE_MAP_ROWS][WAVE_MAP_COLS] = {
		{0,0,1}, {0,2,3}, {0,4,6}, {5,7,9}, {8,10,13}, {11,0,14}, {12,15,17}, {0,16,18}, {0,0,19},
		{0,0,20}, {0,23,21}, {27,24,22}, {28,0,25}, {31,29,26}, {34,32,30}, {0,35,33}, {0,37,36}, {0,0,38}
};
uint8_t distance(int a, int b, int maxlen) {
	uint8_t ab=abs(a-b);
	uint8_t edge=abs(maxlen-ab);
	return min(ab, edge);
}
void effect_wave(sk9822_t *leds, const uint8_t led_count, const uint8_t level) {
	uint8_t led_idx;
	uint8_t dist;
	effect_state = (effect_state+1) % WAVE_LENGTH;
	for (int row=0; row<WAVE_LENGTH; row++) {
		dist = distance(effect_state, row, WAVE_LENGTH);
		if (row>=WAVE_MAP_ROWS)
			continue;

		for (int col=0; col<WAVE_MAP_COLS; col++) {
			led_idx = wave_map[row][col];
			if (led_idx > 0) {
				leds[led_idx-1] = (sk9822_t){.r=0, .g=max(0,dist-WAVE_OFFSET), .b=10, .level=level, .begin=7};
			}

		}
	}
}


/********
 * FIRE *
 ********/
#define FIRE_ROWS 12
#define FIRE_COLS 12
#define FIRE_AVG_WIDTH 2			// how many neighboring cells to use for average
#define FIRE_SEED_VALUE 80			// how bright is the seed
#define FIRE_PROPAGATION_LIMIT 50	// cell value cannot increase more than this
#define FIRE_CHANCE_MASK 7			// 4 bit mask, higher is less likely
const uint8_t fire_map[FIRE_ROWS][FIRE_COLS] = {
		{ 1, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0,38},
		{ 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,37, 0},
		{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,35,36},
		{ 0, 4, 0, 0, 0, 0, 0, 0, 0,34, 0,33},
		{ 6, 0, 5, 0, 0, 0, 0, 0, 0,31,32, 0},
		{ 9, 7, 8, 0, 0, 0, 0, 0, 0, 0,29,30},
		{ 0,10, 0, 0, 0, 0, 0, 0,28, 0,26, 0},
		{13, 0, 0,11,12, 0, 0,27, 0,25, 0, 0},
		{ 0,14, 0, 0, 0, 0,23, 0,24, 0, 0, 0},
		{ 0, 0, 0,15, 0,16, 0, 0, 0,22, 0, 0},
		{ 0, 0,17, 0,18, 0, 0,21, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0,19,20, 0, 0, 0, 0, 0},
};
int8_t fire_img[FIRE_ROWS+1][FIRE_COLS];
void update_fire_image(int8_t fire_img[FIRE_ROWS+1][FIRE_COLS]) {
	uint32_t rand_num = 0;
	uint32_t avg_sum=0;
	uint32_t avg_count=0;
	int avg_col;

	// seed (invisible)
	for (int col=0; col<FIRE_COLS; col++) {
		if (rand_num == 0) {
			rand_num = rand();
		}
		fire_img[0][col] >>= 1; //decay
		fire_img[0][col] += ((rand_num&FIRE_CHANCE_MASK) == FIRE_CHANCE_MASK ? FIRE_SEED_VALUE : 0);
		rand_num>>=4;
	}
	// visible
	for (int row=1; row<=FIRE_ROWS; row++) {
		for (int col=0; col<FIRE_COLS; col++) {
			avg_sum=0;
			avg_count=0;
			for (int i=-FIRE_AVG_WIDTH; i<=FIRE_AVG_WIDTH; i++) {
				avg_col=col+i;
				if (!(avg_col<0 || avg_col>=FIRE_COLS)) {
					avg_sum += fire_img[row-1][avg_col] / (abs(i)+1);
					avg_count++;
				}
			}
			fire_img[row][col]>>=1; //decay
			fire_img[row][col] += min(FIRE_PROPAGATION_LIMIT, max(0,avg_sum/avg_count - row/4));
		}
	}
};

void effect_fire(sk9822_t *leds, const uint8_t led_count, const uint8_t level) {
	uint8_t led_idx;
	uint8_t pixel;
	update_fire_image(fire_img);
	for (int row=0; row<FIRE_ROWS; row++) {
		for (int col=0; col<FIRE_COLS; col++) {
			led_idx = fire_map[row][col];
			pixel = max(fire_img[row+1][col],0);
			if (led_idx > 0) {
				leds[led_idx-1] = (sk9822_t){.r=pixel, .g=max(0,pixel-15)>>2, .b=max(0,pixel-30)>>3, .level=level, .begin=7};
			}
		}
	}
}


/***********
 * SPARKLE *
 ***********/
#define SPARKLE_INTENSITY 128
#define SPARKLE_CHANCE_MASK 63
uint8_t sparkle_mask[LED_COUNT] = {0};
void effect_sparkle(sk9822_t *leds, const uint8_t led_count, const uint8_t level) {
	uint8_t pixel;
	for (uint8_t led_idx = 0; led_idx<led_count; led_idx++) {

		pixel = sparkle_mask[led_idx];
		if (pixel == 0) {
			pixel = ((rand()&SPARKLE_CHANCE_MASK) == SPARKLE_CHANCE_MASK ? SPARKLE_INTENSITY : 0);
		}
		if (pixel > 0) {
			leds[led_idx].b = min(255, leds[led_idx].b + pixel);
			leds[led_idx].g = min(255, leds[led_idx].g + pixel);
			leds[led_idx].r = min(255, leds[led_idx].r + pixel);
			pixel>>=2; //decay
			sparkle_mask[led_idx] = pixel;
		}
	}
}
