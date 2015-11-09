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
} OutdoorState;

typedef struct {
  Layer *root_layer;
  IconTextLayer* icon_text_layers[3];
  OutdoorState outdoor_state;
} CurrentDetailsLayer;

CurrentDetailsLayer* current_details_layer_create_layer(GRect frame, CurrentWeather current_weather);
void current_details_layer_destroy(CurrentDetailsLayer *current_details_layer);
void current_details_layer_set_foreground_color(CurrentDetailsLayer *current_details_layer, GColor foreground_color);
void current_details_layer_set_background_color(CurrentDetailsLayer *current_details_layer, GColor background_color);
void current_details_layer_set_outdoor_state(CurrentDetailsLayer *current_details_layer, CurrentWeather current_weather);
Layer* current_details_layer_get_layer(CurrentDetailsLayer *current_details_layer);