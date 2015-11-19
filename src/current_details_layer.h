#pragma once
#include "icon_text_layer.h"
#include "current_weather_layer.h"

typedef struct {
  time_t sunrise_time;
  time_t sunset_time;
} SunTimeInfo;

typedef struct {
  CurrentWeather current_weather;
  SunTimeInfo sun_time_info;
  time_t current_time;
} OutdoorState;

typedef struct {
  Layer *root_layer;
  IconTextLayer* icon_text_layers[3];
  OutdoorState outdoor_state;
  GColor foreground_color;
  GColor background_color;
} CurrentDetailsLayer;

CurrentDetailsLayer* current_details_layer_create_layer(GRect frame, OutdoorState outdoor_state);
void current_details_layer_destroy(CurrentDetailsLayer *current_details_layer);
void current_details_layer_set_foreground_color(CurrentDetailsLayer *current_details_layer, GColor foreground_color);
void current_details_layer_set_background_color(CurrentDetailsLayer *current_details_layer, GColor background_color);
void current_details_layer_set_outdoor_state(CurrentDetailsLayer *current_details_layer, OutdoorState outdoor_state);
Layer* current_details_layer_get_layer(CurrentDetailsLayer *current_details_layer);