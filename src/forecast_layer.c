#include <pebble.h>
#include "forecast_layer.h"
#include "util.h"
  
static void draw_invalid_forecast(ForecastLayer *forecast_layer, GContext *ctx) {
  // TODO: Draw something about it being invalid
}
  
static void draw_forecast(ForecastLayer *forecast_layer, GContext *ctx) {
  Forecast forecast = forecast_layer->forecast;
  Layer *layer = forecast_layer->layer;
  GRect bounds = layer_get_bounds(layer);
  MinMaxResult temperature_bounds = int16_min_max(forecast.temperatures, 12);
}

static void draw_forecast_layer(Layer *layer, GContext *ctx) {
  ForecastLayer *forecast_layer = (ForecastLayer*) layer_get_data(layer);
  Forecast forecast = forecast_layer->forecast;
  
  if(forecast.valid) {
    draw_forecast(forecast_layer, ctx);
  } else {
    draw_invalid_forecast(forecast_layer, ctx);
  }
}

ForecastLayer* forecast_layer_create_layer(GRect frame, Forecast forecast) {
  Layer *layer = layer_create_with_data(frame, sizeof(ForecastLayer));
  layer_set_hidden(layer, true);
  layer_set_update_proc(layer, draw_forecast_layer);

  ForecastLayer *forecast_layer = layer_get_data(layer);
  forecast_layer->forecast = forecast;
  forecast_layer->background_color = GColorWhite;
  forecast_layer->foreground_color = GColorBlack;

  return forecast_layer;
}

void forecast_layer_destroy(ForecastLayer *forecast_layer) {
  layer_destroy(forecast_layer->layer);
}

Layer* forecast_layer_get_layer(ForecastLayer *forecast_layer) {
  return forecast_layer->layer;
}

void forecast_layer_set_foreground_color(ForecastLayer *forecast_layer, GColor foreground_color) {
  forecast_layer->foreground_color = foreground_color;
}

void forecast_layer_set_forecast(ForecastLayer *forecast_layer, Forecast forecast) {
  forecast_layer->forecast = forecast;
}

void forecast_layer_set_hidden(ForecastLayer *forecast_layer, bool hidden) {
  layer_set_hidden(forecast_layer->layer, hidden);
}