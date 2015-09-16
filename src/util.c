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
