#include <pebble.h>
#include "bluetooth_layer.h"
#include "util.h"
  
static void draw_bluetooth(Layer *layer, GContext *ctx) {
  BluetoothLayer *bluetooth_layer = layer_get_data(layer);
  
  if(bluetooth_layer->connected) {
    graphics_context_set_stroke_color(ctx, bluetooth_layer->foreground_color);
    gpath_draw_outline(ctx, bluetooth_layer->path);
  }
}

static GPath* generate_bluetooth_path(const GRect* rect) {
  int16_t height = rect->size.h;
  int16_t width = rect->size.w;
  
  uint16_t one_third_width = scale_length(width, 0.33f);
  uint16_t two_thirds_width = scale_length(width, 0.66f);
  uint16_t half_height = scale_length(height, 0.50f);
  
  uint32_t size = 6;
  GPoint *points = malloc(sizeof(GPoint) * size);
  points[0] = (GPoint) {one_third_width, height};
  points[1] = (GPoint) {two_thirds_width, 0};
  points[2] = (GPoint) {width, half_height};
  points[3] = (GPoint) {0, half_height};
  points[4] = (GPoint) {one_third_width, 0};
  points[5] = (GPoint) {two_thirds_width, height};
  
  GPathInfo path_info = {
    .num_points = size,
    .points = points
  };
  
  return gpath_create(&path_info);
}

BluetoothLayer* bluetooth_layer_create_layer(GRect frame, bool connected) {
  Layer *layer = layer_create_with_data(frame, sizeof(BluetoothLayer));
  BluetoothLayer *bluetooth_layer = (BluetoothLayer*) layer_get_data(layer);
  
  *bluetooth_layer = (BluetoothLayer){
    .foreground_color = GColorWhite,
    .layer = layer,
    .connected = connected,
    .path = generate_bluetooth_path(&frame),
  };
  
  layer_set_update_proc(layer, draw_bluetooth);
  
  return bluetooth_layer;
}
  
void bluetooth_layer_destroy(BluetoothLayer *bluetooth_layer) {
  free(bluetooth_layer->path->points);
  gpath_destroy(bluetooth_layer->path);
  layer_destroy(bluetooth_layer->layer);
}

void bluetooth_layer_set_bluetooth_state(BluetoothLayer *bluetooth_layer, bool connected) {
  bluetooth_layer->connected = connected;
  layer_mark_dirty(bluetooth_layer->layer);
}

void bluetooth_layer_set_foreground_color(BluetoothLayer *bluetooth_layer, GColor foreground_color) {
  bluetooth_layer->foreground_color = foreground_color;
}

Layer* bluetooth_layer_get_layer(BluetoothLayer *bluetooth_layer) {
  return bluetooth_layer->layer;
}