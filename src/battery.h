#pragma once
#include <pebble.h>

typedef struct {
  BatteryChargeState battery_state;
  Layer *layer;
  GColor background_color;
  GColor foreground_color;
  GPathInfo *bolt_path;
} BatteryLayer;
  
BatteryLayer* battery_layer_create_layer(GRect frame);
void battery_layer_destroy(BatteryLayer *layer);
Layer* battery_layer_get_layer(BatteryLayer *layer);
void battery_layer_set_background_color(BatteryLayer *layer, GColor background_color);
void battery_layer_set_foreground_color(BatteryLayer *layer, GColor foreground_color);
void battery_layer_set_battery_state(BatteryLayer *battery_layer, BatteryChargeState state);