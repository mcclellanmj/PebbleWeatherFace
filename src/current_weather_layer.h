#pragma once

typedef struct {
  uint8_t temperature;
  enum {NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST} wind_dir;
  uint8_t wind_speed;
} WeatherShortDetail;
  
typedef struct {
  enum {AVAILABLE, ERROR, NOT_REQUESTED, REQUESTING} status;
  WeatherShortDetail detail;
} WeatherShort; 

typedef struct {
  Layer *layer;
  WeatherShort current_weather;
  GColor foreground_color;
  GColor background_color;
} CurrentWeatherLayer;


CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, WeatherShort current_weather);
void current_weather_layer_destroy(CurrentWeatherLayer *current_weather_layer);
Layer* current_weather_layer_get_layer(CurrentWeatherLayer *current_weather_layer);
void current_weather_layer_set_foreground_color(CurrentWeatherLayer *current_weather_layer, GColor foreground_color);
void current_weather_layer_set_bluetooth_state(CurrentWeatherLayer *current_weather_layer, WeatherShort current_weather);