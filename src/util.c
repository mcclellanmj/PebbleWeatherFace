#include "util.h"

uint16_t scale_length(uint16_t max_size, float percentage) {
  return ((float) max_size) * percentage;
}

uint16_t scale_length_using_int(uint16_t max_size, uint8_t percentage) {
  float as_decimal_percent = ((float) percentage) / 100.0;
  return scale_length(max_size, as_decimal_percent);
}

GPoint gpoint_add(const GPoint *x, const GPoint *y) {
  return GPoint(x->x + y->x, x->y + y->y);
}

typedef struct {
  bool valid;
  int16_t min;
  int16_t max;
} MinMaxResult;

MinMaxResult int16_min_max(int16_t *array, size_t num_elements) {
  if(num_elements <= 0) {
    return { .valid = false };
  }

  MinMaxResult minMaxResult;
  minMaxResult.min = array[0];
  minMaxResult.max = array[0];

  for(int i = 0; i < num_elements; i++) {
    int16_t value = array[i];
    if(value > minMaxResult.max) {
      minMaxResult.max = value;
    }

    if(value < minMaxResult.min) {
      minMaxResult.min = value;
    }
  }

  return minMaxResult;
}
