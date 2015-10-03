#pragma once
  
typedef struct {
  bool valid;
  uint8_t start_time;
  int16_t temperatures[12];
  // TODO: chance_of_rain could be a uint8_t
  int16_t chance_of_rain[12];
} Forecast;

typedef struct {
  Forecast forecast;
  Layer* layer;
  GColor foreground_color;
  GColor background_color;
} ForecastLayer;

ForecastLayer* forecast_layer_create_layer(GRect frame, Forecast forecast);
void forecast_layer_destroy(ForecastLayer *forecast_layer);
Layer* forecast_layer_get_layer(ForecastLayer *forecast_layer);
void forecast_layer_set_foreground_color(ForecastLayer *forecast_layer, GColor foreground_color);
void forecast_layer_set_forecast(ForecastLayer *forecast_layer, Forecast forecast);
void forecast_layer_set_hidden(ForecastLayer *forecast_layer, bool hidden);