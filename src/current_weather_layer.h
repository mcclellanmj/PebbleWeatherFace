#pragma once
#include <pebble.h>
#include "bitmap_container.h"
#include "copying_text_layer.h"

enum WeatherStatus {AVAILABLE, ERROR};

typedef struct {
  enum WeatherStatus status;
  bool valid;
  int16_t temperature;
  uint8_t icon_offset;
  uint8_t uv_index;
  uint8_t humidity;
  uint8_t wind_speed;
} CurrentWeather;

typedef struct {
    BitmapContainer* bitmap_container;
} LazyLoadState;

typedef struct {
  Layer *layer;
  BitmapLayer *icon_layer;
  CopyingTextLayer *temperature_layer;
  GColor foreground_color;
  GColor background_color;
  LazyLoadState initialized_state;
} CurrentWeatherLayer;


CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, CurrentWeather current_weather);
void current_weather_layer_destroy(CurrentWeatherLayer *current_weather_layer);
void current_weather_layer_set_foreground_color(CurrentWeatherLayer *current_weather_layer, GColor foreground_color);
void current_weather_layer_set_weather(CurrentWeatherLayer *current_weather_layer, CurrentWeather current_weather);
Layer* current_weather_layer_get_layer(CurrentWeatherLayer *current_weather_layer);
