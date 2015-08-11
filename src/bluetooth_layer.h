#pragma once
#include <pebble.h>

typedef struct {
  bool connected;
  Layer* layer;
  GColor foreground_color;
  GPath* path;
} BluetoothLayer;
  
BluetoothLayer* bluetooth_layer_create_layer(GRect frame, bool connected);
void bluetooth_layer_destroy(BluetoothLayer *bluetooth_layer);
Layer* bluetooth_layer_get_layer(BluetoothLayer *bluetooth_layer);
void bluetooth_layer_set_foreground_color(BluetoothLayer *bluetooth_layer, GColor foreground_color);
void bluetooth_layer_set_bluetooth_state(BluetoothLayer *bluetooth_layer, bool connected);
