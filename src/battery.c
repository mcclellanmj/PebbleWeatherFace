#include <pebble.h>
#include "battery.h"
#include "util.h"

Layer *battery_layer_get_layer(BatteryLayer *battery_layer) {
  return battery_layer->layer;
}

static GPathInfo* generate_bolt_path(GRect rect) {
  uint16_t height = rect.size.h;
  // uint16_t width = rect.size.w; // Needs to be used
  uint16_t left_bolt = scale_length(height, 0.58f);
  uint16_t right_bolt = scale_length(height, 0.42f);
  
  uint8_t size = 7;
  GPoint *points = malloc(sizeof(GPoint) * size);
  
  points[6] = (GPoint) {4,0};
  points[5] = (GPoint) {0, left_bolt};
  points[4] = (GPoint) {4, left_bolt};
  points[3] = (GPoint) {4, height};
  points[2] = (GPoint) {8, right_bolt};
  points[1] = (GPoint) {4, right_bolt};
  points[0] = (GPoint) {4, 0};
  
  struct GPathInfo *path_info = malloc(sizeof(GPathInfo));
  *path_info = (GPathInfo) {
    .num_points = size,
    .points = points
  };
  
  return path_info;
}

static void draw_battery(Layer *layer, GContext *ctx) {
  BatteryLayer* battery_layer = (BatteryLayer*) layer_get_data(layer);
  
  // Calculate all the boundaries
  GRect full_bounds = layer_get_bounds(layer);
  GRect battery_outline = GRect(full_bounds.origin.x, full_bounds.origin.y, full_bounds.size.w - 2, full_bounds.size.h);
  GRect battery_nipple = GRect(battery_outline.size.w, (full_bounds.size.h/2) - 3, 2, (full_bounds.size.h/2) + 1);
  GRect battery_level_bounds = grect_crop(battery_outline, 3);
  
  battery_level_bounds.size.w = scale_length_using_int(battery_level_bounds.size.w, battery_layer->battery_state.charge_percent);
  
  // Draw the outside of the battery
  graphics_context_set_stroke_color(ctx, battery_layer->foreground_color);
  graphics_draw_round_rect(ctx, battery_outline, 2);
  
  // Draw battery level
  graphics_context_set_fill_color(ctx, battery_layer->foreground_color);
  graphics_fill_rect(ctx, battery_level_bounds, 2, GCornersAll);
  
  // Draw battery nipple
  graphics_fill_rect(ctx, battery_nipple, 2, GCornerNone);
  
  if(battery_layer->battery_state.is_charging) {
    GPath *path = gpath_create(battery_layer->bolt_path);
    
    // TODO: needs to move a percentage and be precalculated
    gpath_move_to(path, GPoint(5, 0));
    
    gpath_draw_filled(ctx, path);
    
    graphics_context_set_stroke_color(ctx, battery_layer->background_color);
    gpath_draw_outline(ctx, path);
    
    gpath_destroy(path);
  }
}

BatteryLayer* battery_layer_create_layer(GRect rect) {
  Layer *layer = layer_create_with_data(rect, sizeof(BatteryLayer));
  
  BatteryLayer *battery_layer = (BatteryLayer*) layer_get_data(layer);
  *battery_layer = (BatteryLayer) {
    .background_color = GColorBlack,
    .foreground_color = GColorWhite,
    .layer = layer,
    .bolt_path = generate_bolt_path(rect),
  };
  
  layer_set_clips(layer, false);
  layer_set_update_proc(battery_layer->layer, draw_battery);
  
  return battery_layer;
}

void battery_layer_set_battery_state(BatteryLayer *battery_layer, BatteryChargeState state) {
  battery_layer->battery_state = state;
  layer_mark_dirty(battery_layer->layer);
}

void battery_layer_destroy(BatteryLayer *battery_layer) {
  GPathInfo *info = battery_layer->bolt_path;
  free(info->points);
  free(info);
  layer_destroy(battery_layer->layer);
}

void battery_layer_set_background_color(BatteryLayer *layer, GColor background_color) {
  layer->background_color = background_color;
}

void battery_layer_set_foreground_color(BatteryLayer *layer, GColor foreground_color) {
  layer->foreground_color = foreground_color;
}