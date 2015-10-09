#pragma once
#include <pebble.h>

enum WeatherStatus {AVAILABLE, ERROR};
enum WindDir {NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST};

typedef struct {
  enum WeatherStatus status;
  int16_t temperature;
  int16_t wind_dir;
  uint8_t icon_offset;
  int16_t wind_speed;
} CurrentWeather;

typedef struct {
    GPathInfo* arrow_path;
    GBitmap* icons_bitmap;
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
