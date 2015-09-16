#pragma once
#include <pebble.h>

uint16_t scale_length(uint16_t max_size, float percentage);
uint16_t scale_length_using_int(uint16_t max_size, uint8_t percentage);
GPoint gpoint_add(const GPoint *x, const GPoint *y);
