#include <pebble.h>
#include "forecast_layer.h"

ForecastLayer* forecast_layer_create_layer(GRect frame, Forecast forecast);
void forecast_layer_destroy(ForecastLayer *forecast_layer);
Layer* forecast_layer_get_layer(ForecastLayer *forecast_layer);
void forecast_layer_set_foreground_color(ForecastLayer *forecast_layer, GColor foreground_color);
void forecast_layer_set_forecast(ForecastLayer *forecast_layer, Forecast forecast);

void forecast_layer_set_hidden(ForecastLayer *forecast_layer, bool hidden) {
  layer_set_hidden(forecast_layer->layer, hidden);
}