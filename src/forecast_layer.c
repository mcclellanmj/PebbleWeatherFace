#include <pebble.h>
#include "forecast_layer.h"
#include "util.h"
  
static void draw_invalid_forecast(ForecastLayer *forecast_layer, GContext *ctx) {
  Layer *layer = forecast_layer->layer;
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, forecast_layer->background_color);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  graphics_context_set_text_color(ctx, forecast_layer->foreground_color);

  // TODO: Text needs to be moved down about 20 pixels
  graphics_draw_text(ctx,
                     "X",
                     fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD),
                     bounds,
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

static void draw_small_text(GContext *ctx, char* text, GRect location) {
  graphics_draw_text(ctx,
                     text,
                     fonts_get_system_font(FONT_KEY_GOTHIC_14),
                     location,
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}
  
static void draw_forecast(ForecastLayer *forecast_layer, GContext *ctx) {
  Forecast forecast = forecast_layer->forecast;
  Layer *layer = forecast_layer->layer;
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, forecast_layer->background_color);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  MinMaxResult temperature_bounds = int16_min_max(forecast.temperatures, 12);
  
  char max_temperature[5];
  snprintf(max_temperature, 5, "%d", temperature_bounds.max);
  
  char min_temperature[5];
  snprintf(min_temperature, 5, "%d", temperature_bounds.min);
  
  graphics_context_set_fill_color(ctx, forecast_layer->foreground_color);
  
  GRect high_rect = GRect(132, 2, 12, 18);
  graphics_fill_rect(ctx, high_rect, 0, GCornerNone);
  GRect low_rect = GRect(132, 72, 12, 18);
  graphics_fill_rect(ctx, low_rect, 0, GCornerNone);
    
    
  graphics_context_set_text_color(ctx, forecast_layer->background_color);
  draw_small_text(ctx, max_temperature, high_rect);
  draw_small_text(ctx, min_temperature, low_rect);
}

static void draw_forecast_layer(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing the forecast");

  ForecastLayer *forecast_layer = (ForecastLayer*) layer_get_data(layer);
  Forecast forecast = forecast_layer->forecast;

  if(forecast.valid == true) {
    draw_forecast(forecast_layer, ctx);
  } else {
    draw_invalid_forecast(forecast_layer, ctx);
  }
}

ForecastLayer* forecast_layer_create_layer(GRect frame, Forecast forecast) {
  Layer *layer = layer_create_with_data(frame, sizeof(ForecastLayer));


  ForecastLayer *forecast_layer = (ForecastLayer*) layer_get_data(layer);
  *forecast_layer = (ForecastLayer) {
      .forecast = forecast,
      .layer = layer,
      .background_color = GColorWhite,
      .foreground_color = GColorBlack
  };

  layer_set_hidden(layer, true);
  layer_set_update_proc(layer, draw_forecast_layer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Forecast layer has been created");

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