#pragma once
#include "icon_text_layer.h"
#include "current_weather_layer.h"

#define NUMBER_OF_DETAIL_LAYERS 4

typedef struct {
  bool valid;
  time_t sunrise_time;
  time_t sunset_time;
} SunTimeInfo;

typedef struct {
  bool valid;
  CurrentWeather current_weather;
  SunTimeInfo sun_time_info;
  time_t current_time;
} OutdoorState;

typedef struct {
  Layer *root_layer;
  IconTextLayer* icon_text_layers[NUMBER_OF_DETAIL_LAYERS];
  OutdoorState outdoor_state;
  GColor foreground_color;
  GColor background_color;
  uint8_t active_layers;
} CurrentDetailsLayer;

CurrentDetailsLayer* current_details_layer_create_layer(GRect frame, OutdoorState outdoor_state);
void current_details_layer_destroy(CurrentDetailsLayer *current_details_layer);
void current_details_layer_set_foreground_color(CurrentDetailsLayer *current_details_layer, GColor foreground_color);
void current_details_layer_set_background_color(CurrentDetailsLayer *current_details_layer, GColor background_color);
void current_details_layer_set_outdoor_state(CurrentDetailsLayer *current_details_layer, OutdoorState outdoor_state);
Layer* current_details_layer_get_layer(CurrentDetailsLayer *current_details_layer);