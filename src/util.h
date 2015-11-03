#pragma once
#include <pebble.h>

typedef struct {
  bool valid;
  int16_t min;
  int16_t max;
} MinMaxResult;

char* copy_string(const char* source, size_t max);
MinMaxResult int16_min_max(const int16_t *array, size_t num_elements);
uint16_t scale_length(uint16_t max_size, float percentage);
uint16_t scale_length_using_int(uint16_t max_size, uint8_t percentage);
GPoint gpoint_add(const GPoint *x, const GPoint *y);
