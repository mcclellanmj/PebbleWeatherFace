#include <pebble.h>
#include "drawing.h"

void graphics_draw_horizontal_dotted_line(GContext *ctx, uint8_t start_x, uint8_t end_x, uint8_t y_coord, uint8_t dash_length, uint8_t distance_between) {
  for(uint8_t i = start_x; i < end_x; i=i + (dash_length + distance_between)) {
    GPoint start_dash = GPoint(i, y_coord);
    GPoint end_dash = GPoint(i + dash_length, y_coord);
    graphics_draw_line(ctx, start_dash, end_dash);  
  }
}