#pragma once
#include <pebble.h>
#include "bitmap_container.h"

enum WeatherStatus {AVAILABLE, ERROR};

typedef struct {
  enum WeatherStatus status;
  int16_t temperature;
  uint8_t icon_offset;
  int16_t uv_index;
} CurrentWeather;

typedef struct {
    BitmapContainer* bitmap_container;
} LazyLoadState;

typedef struct {
  Layer* layer;
  CurrentWeather current_weather;
  GColor foreground_color;
  GColor background_color;
  LazyLoadState initialized_state;
} CurrentWeatherLayer;


CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, CurrentWeather current_weather);
void current_weather_layer_destroy(CurrentWeatherLayer *current_weather_layer);
void current_weather_layer_set_foreground_color(CurrentWeatherLayer *current_weather_layer, GColor foreground_color);
void current_weather_layer_set_weather(CurrentWeatherLayer *current_weather_layer, CurrentWeather current_weather);
Layer* current_weather_layer_get_layer(CurrentWeatherLayer *current_weather_layer);
