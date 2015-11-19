#include "util.h"

// TODO: Needs to use malloc instead but requires some more testing
char* copy_string(const char* source, size_t max) {
  size_t string_length = strlen(source) + 1;
  size_t largest = string_length > max + 1 ? max + 1 : string_length;

  char* destination = (char *) calloc(sizeof(char), largest);
  strncpy(destination, source, largest - 1);
  return destination;
}

uint16_t scale_length(uint16_t max_size, float percentage) {
  return ((float) max_size) * percentage;
}

uint16_t scale_length_using_int(uint16_t max_size, uint8_t percentage) {
  float as_decimal_percent = ((float) percentage) / 100.0;
  return scale_length(max_size, as_decimal_percent);
}

GPoint gpoint_add(const GPoint *first, const GPoint *second) {
  return GPoint(first->x + second->x, first->y + second->y);
}

MinMaxResult int16_min_max(const int16_t *array, size_t num_elements) {
  if(num_elements <= 0) {
    return (MinMaxResult) { .valid = false };
  }

  MinMaxResult minMaxResult;
  minMaxResult.valid = true;
  minMaxResult.min = array[0];
  minMaxResult.max = array[0];

  for(size_t i = 0; i < num_elements; i++) {
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
