#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "stdint.h"

#define LED_COUNT 				38

typedef union {
	uint32_t raw;
	struct {
		unsigned char level: 5;
		unsigned char begin: 3;
		unsigned char b;
		unsigned char g;
		unsigned char r;
	};
} __attribute__ ((aligned (4), packed)) sk9822_t;
typedef void (*effect_function_t)(sk9822_t*, uint8_t, uint8_t);

void effect_pulse_red(sk9822_t *leds, const uint8_t led_count, const uint8_t level);
void effect_pulse_green(sk9822_t *leds, const uint8_t led_count, const uint8_t level);
void effect_pulse_blue(sk9822_t *leds, const uint8_t led_count, const uint8_t level);
void effect_wave(sk9822_t *leds, const uint8_t led_count, const uint8_t level);
void effect_fire(sk9822_t *leds, const uint8_t led_count, const uint8_t level);
void effect_sparkle(sk9822_t *leds, const uint8_t led_count, const uint8_t level);


#endif /* __EFFECTS_H__ */
