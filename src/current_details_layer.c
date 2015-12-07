#include <pebble.h>
#include "current_details_layer.h"
#include "icon_text_layer.h"

static const int POTENTIAL_LAYER_COUNT = 5;

typedef struct {
  bool (*is_available)(const OutdoorState *state);
  IconTextLayer* (*create)(GRect frame, const OutdoorState *state);
} PotentialIcons;

// Start helper methods
static IconTextLayer* create_time_icon_layer(GRect frame, GBitmap* bitmap, const time_t *time_in_millis) {
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
static bool sunrise_is_available(const OutdoorState *state) {
  time_t current_time = state->current_time;
  return !is_sun_up(current_time, state->sun_time_info);
}

static IconTextLayer* sunrise_create(GRect frame, const OutdoorState *state) {
  return create_time_icon_layer(frame, gbitmap_create_with_resource(RESOURCE_ID_SUN_RISE_ICON), &state->sun_time_info.sunrise_time);
}
// End sunrise layer

// UV index layer
static bool uv_is_available(const OutdoorState *state) {
  return is_sun_up(state->current_time, state->sun_time_info);
}

static IconTextLayer* uv_create(GRect frame, const OutdoorState *state) {
  char uv[3];
  snprintf(uv, 3, "%d", state->current_weather.uv_index);
  return icon_text_layer_create(frame, gbitmap_create_with_resource(RESOURCE_ID_UV_ICON), uv, 2);
}
// End UV layer

// Sunset layer
static bool sunset_is_available(const OutdoorState *state) {
  return is_sun_up(state->current_time, state->sun_time_info);
}

static IconTextLayer* sunset_create(GRect frame, const OutdoorState *state) {
  return create_time_icon_layer(frame, gbitmap_create_with_resource(RESOURCE_ID_SUN_SET_ICON), &state->sun_time_info.sunset_time);
}
// End sunset layer

// Wind layer
static bool wind_is_available(const OutdoorState *state) {
  return true;
}

static IconTextLayer* wind_create(GRect frame, const OutdoorState *state) {
  char wind_speed[8];
  snprintf(wind_speed, 7, "%d mph", state->current_weather.wind_speed);
  return icon_text_layer_create(frame, gbitmap_create_with_resource(RESOURCE_ID_WIND_ICON), wind_speed, 7);
}

// End Wind

// Humidity layer
static bool humidity_is_available(const OutdoorState *state) {
  return true;
}

static IconTextLayer* humidity_create(GRect frame, const OutdoorState *state) {
  char humidity[5];
  snprintf(humidity, 5, "%d%%", state->current_weather.humidity);
  return icon_text_layer_create(frame, gbitmap_create_with_resource(RESOURCE_ID_HUMIDITY_ICON), humidity, 4);
}
// End Humidity

static PotentialIcons potential_icons[5] = {
  { .is_available = sunrise_is_available
  ,  .create = sunrise_create
  },
  { .is_available = sunset_is_available
  , .create = sunset_create
  },
  { .is_available = uv_is_available
  , .create = uv_create
  },
  { .is_available = wind_is_available
  , .create = wind_create
  },
  { .is_available = humidity_is_available
  , .create = humidity_create
  }
};

static uint8_t create_icon_text_layers(Layer *root_layer, IconTextLayer *icon_text_layers[4], OutdoorState outdoor_state) {
  uint8_t current_place = 0;
  for(int i = 0; i < POTENTIAL_LAYER_COUNT && current_place < NUMBER_OF_DETAIL_LAYERS; i++) {
    if(potential_icons[i].is_available(&outdoor_state)) {
      GRect location = GRect(0, 5 + (8 * current_place), 94, 100);
      icon_text_layers[current_place] = potential_icons[i].create(location, &outdoor_state);
      layer_add_child(root_layer, icon_text_layer_get_layer(icon_text_layers[current_place]));
      current_place = current_place + 1;
    }
  }
  
  return current_place;
}

CurrentDetailsLayer* current_details_layer_create_layer(GRect frame, OutdoorState outdoor_state) {
  Layer *root_layer = layer_create_with_data(frame, sizeof(CurrentDetailsLayer));
  CurrentDetailsLayer *current_details_layer = (CurrentDetailsLayer *) layer_get_data(root_layer);
  *current_details_layer = (CurrentDetailsLayer) {
    .root_layer = root_layer,
    .active_layers = 0
  };
  
  current_details_layer_set_outdoor_state(current_details_layer, outdoor_state);
  return current_details_layer;
}

static void destroy_icon_text_layers(IconTextLayer* icon_text_layers[NUMBER_OF_DETAIL_LAYERS], uint8_t active_layers) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroying the layers");
  for(int i = 0; i < active_layers; i++) {
    IconTextLayer *icon_text_layer = icon_text_layers[i];
    layer_remove_from_parent(icon_text_layer_get_layer(icon_text_layer));
    icon_text_layer_destroy(icon_text_layer);
  }
}

void current_details_layer_destroy(CurrentDetailsLayer *current_details_layer) {
  destroy_icon_text_layers(current_details_layer->icon_text_layers, current_details_layer->active_layers);
  layer_destroy(current_details_layer->root_layer);
}

void current_details_layer_set_foreground_color(CurrentDetailsLayer *current_details_layer, GColor foreground_color) {
  current_details_layer->foreground_color = foreground_color;
}

void current_details_layer_set_background_color(CurrentDetailsLayer *current_details_layer, GColor background_color) {
  current_details_layer->background_color = background_color;
}

void current_details_layer_set_outdoor_state(CurrentDetailsLayer *current_details_layer, OutdoorState outdoor_state) {
  destroy_icon_text_layers(current_details_layer->icon_text_layers, current_details_layer->active_layers);
  current_details_layer->active_layers = 0;
  if(outdoor_state.valid) {
    current_details_layer->active_layers = create_icon_text_layers(current_details_layer->root_layer, current_details_layer->icon_text_layers, outdoor_state);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got an invalid outdoor_state, skipped creation of the new layers");
  }
}

Layer* current_details_layer_get_layer(CurrentDetailsLayer *current_details_layer) {
  return current_details_layer->root_layer;
}