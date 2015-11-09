#include <pebble.h>
#include "current_details_layer.h"
#include "icon_text_layer.h"

typedef struct {
  time_t current_time;
  OutdoorState outdoor_state;
} PotentialDetailState;

typedef struct {
  bool (*is_available)(const PotentialDetailState *state);
  IconTextLayer* (*create)(GRect frame, const PotentialDetailState *state);
} PotentialIcons;

// Start helper methods
static IconTextLayer* create_time_icon_layer(GRect frame, const GBitmap* bitmap, const time_t *time_in_millis) {
  char time[6];
  struct tm *time_info = localtime(time_in_millis);
  strftime(time, 6, "%H:%M", time_info);
  return icon_text_layer_create(frame, bitmap, time, 6);
}

static bool is_sun_up(const time_t current_time, const SunTimeInfo sun_time_info) {
  return current_time >= sun_time_info.sunrise_time && current_time < sun_time_info.sunset_time;
}
// End helper methods

// Sunrise layer
static bool sunrise_is_available(const PotentialDetailState *state) {
  time_t current_time = state->current_time;
  return is_sun_up(current_time, state->outdoor_state.sun_time_info);
}

static IconTextLayer* sunrise_create(GRect frame, const PotentialDetailState *state) {
  return create_time_icon_layer(frame, gbitmap_create_with_resource(RESOURCE_ID_SUN_RISE_ICON), &state->sun_time_info.sunrise_time);
}
// End sunrise layer

// UV index layer
static bool uv_is_available(const PotentialDetailState *state) {
  return is_sun_up(state->current_time, state->outdoor_state.sun_time_info);
}

static IconTextLayer* uv_create(GRect frame, const PotentialDetailState *state) {
  return NULL;
}
// End UV layer

// Sunset layer
static bool sunset_is_available(const PotentialDetailState *state) {
  return !is_sun_up(current_time, state->outdoor_state.sun_time_info);
}

static IconTextLayer* sunset_create(GRect frame, const PotentialDetailState *state) {
  return create_time_icon_layer(frame, gbitmap_create_with_resource(RESOURCE_ID_SUN_SET_ICON), &state->sun_time_info.sunset_time);
}
// End sunset layer

static PotentialIcons potential_icons[3] = {
  { .is_available = sunrise_is_available
  ,  .create = sunrise_create
  },
  { .is_available = sunset_is_available
  , .create = sunset_create
  },
  { .is_available = uv_is_available
  , .create = uv_create
  },
};

static void create_icon_text_layers(IconTextLayer** icon_text_layers, CurrentWeather current_weather) {
  *icon_text_layers = NULL;
}

CurrentDetailsLayer* current_details_layer_create_layer(GRect frame, CurrentWeather current_weather) {
  Layer *root_layer = layer_create_with_data(frame, sizeof(CurrentDetailsLayer));
  CurrentDetailsLayer *current_details_layer = (CurrentDetailsLayer *) layer_get_data(root_layer);
  *current_details_layer = (CurrentDetailsLayer) {
    .root_layer = root_layer,
  };
  
  create_icon_text_layers(current_details_layer->icon_text_layers, current_weather);
  return current_details_layer;
}

void current_details_layer_destroy(CurrentDetailsLayer *current_details_layer) {
  for(int i = 0; i < 3; i++) {
    icon_text_layer_destroy(current_details_layer->icon_text_layers[i]);
  } 
  
  layer_destroy(current_details_layer->root_layer);
}

void current_details_layer_set_foreground_color(CurrentDetailsLayer *current_details_layer, GColor foreground_color) {
  current_details_layer->foreground_color = foreground_color;
}

void current_details_layer_set_background_color(CurrentDetailsLayer *current_details_layer, GColor background_color) {
  current_details_layer->background_color = background_color;
}

void current_details_layer_set_outdoor_state(CurrentDetailsLayer *current_details_layer, OutdoorState outdoor_state) {
}

Layer* current_details_layer_get_layer(CurrentDetailsLayer *current_details_layer) {
  return NULL;
}