#include "util.h"

// TODO: make two versions of this with a slightly more efficient already floating percentage
uint16_t scale_length(uint16_t max_size, uint8_t percentage) {
  float as_decimal_percent = ((float) percentage) / 100.0;
  return ((float) max_size) * as_decimal_percent;
}