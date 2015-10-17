#pragma once
#include <pebble.h>

void graphics_draw_horizontal_dotted_line(GContext *ctx, uint8_t start_x, uint8_t end_x, uint8_t y_coord, uint8_t dash_length, uint8_t distance_between);