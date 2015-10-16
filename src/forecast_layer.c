#include <pebble.h>
#include "forecast_layer.h"
#include "util.h"
#include "drawing.h"
  
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
                     GRect(bounds.origin.x, bounds.origin.y + 20, bounds.size.w, bounds.size.h),
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

uint16_t scale_to_range(int16_t value, int16_t old_min, int16_t old_max, int16_t new_min) {
  return ( (value - old_min) / (float) (old_max - old_min) ) * (7 - new_min) + new_min;
}

void draw_temperature_line(GContext *ctx, const int16_t *temperatures, GRect graph_bounds) {
  MinMaxResult temperature_bounds = int16_min_max(temperatures, 12);
  if(temperature_bounds.valid == false) {
    return;
  }

  uint16_t lastX = 0;
  uint16_t lastY = scale_to_range(temperatures[0], temperature_bounds.min, temperature_bounds.max, graph_bounds.size.h - 7);

  for(int i = 1; i < 12; i++) {
    uint16_t x = scale_length(graph_bounds.size.w, i/11.0);
    uint16_t y = scale_to_range(temperatures[i], temperature_bounds.min, temperature_bounds.max, graph_bounds.size.h - 7);
    graphics_draw_line(ctx, GPoint(lastX, lastY), GPoint(x, y));
    lastX = x;
    lastY = y;
  }
}

void draw_dashed_lines(GContext *ctx, GRect graph_bounds) {
  uint16_t lower_point = graph_bounds.origin.y + graph_bounds.size.h;
  uint16_t twenty_percent = scale_length(graph_bounds.size.h - 7, .2);
  
  for(uint8_t i = 1; i < 5; i++) {
    graphics_draw_dotted_line(ctx, graph_bounds.origin.x, graph_bounds.size.w, lower_point - (twenty_percent * i), 2, 3);
  }
}

void draw_rain_bars(GContext *ctx, const uint8_t *rain_chances, GRect graph_bounds) {
  // Draw the chance of rain bars
  for(int i = 0; i < 12; i++) {
    uint16_t x = scale_length(graph_bounds.size.w, i/11.0);
    uint16_t top = scale_length(graph_bounds.size.h - 7, rain_chances[i] * .01);

    graphics_fill_rect(ctx, GRect(x - 2, graph_bounds.size.h, 5, 0 - top), 0, GCornerNone);
  }
}
  
static void draw_forecast(ForecastLayer *forecast_layer, GContext *ctx) {
  Forecast forecast = forecast_layer->forecast;
  Layer *layer = forecast_layer->layer;
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, forecast_layer->background_color);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  MinMaxResult temperature_bounds = int16_min_max(forecast.temperatures, 12);
  graphics_context_set_text_color(ctx, forecast_layer->foreground_color);

  // Draw the first time in top left
  char time[3];
  snprintf(time, 3, "%d", forecast.start_time);
  draw_small_text(ctx, time, GRect(0, 0, 20, 20));

  // Draw the vertical lines to help see the temps
  graphics_context_set_stroke_color(ctx, forecast_layer->foreground_color);
  uint16_t y = scale_length(bounds.size.h, .70);
  for(int i = 1; i < 4; i++) {
    uint16_t x = scale_length(bounds.size.w, i * (3/11.0));
    GPoint top = GPoint(x, 0);
    GPoint bottom = GPoint(x, y);
    graphics_draw_line(ctx, top, bottom);

    // Draw the time offset to the right
    snprintf(time, 3, "%d", (forecast.start_time + (i * 3)) % 24);
    draw_small_text(ctx, time, GRect(x, 0, 20, 20));
  }

  graphics_context_set_fill_color(ctx, forecast_layer->foreground_color);
  graphics_fill_rect(ctx, GRect(0, y, bounds.size.w, 2), 0, GCornerNone);

  char high_low[10];
  snprintf(high_low, 10, "%d/%d", temperature_bounds.max, temperature_bounds.min);
  graphics_draw_text(ctx,
                     high_low,
                     fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
                     GRect(bounds.size.w - 60, y, 60, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  GRect graph_bounds = GRect(0, 0, bounds.size.w, y);
  draw_temperature_line(ctx, forecast.temperatures, graph_bounds);
  draw_rain_bars(ctx, forecast.chance_of_rain, graph_bounds);
  draw_dashed_lines(ctx, graph_bounds);
}

static void draw_forecast_layer(Layer *layer, GContext *ctx) {
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
  layer_mark_dirty(forecast_layer->layer);
}

void forecast_layer_set_hidden(ForecastLayer *forecast_layer, bool hidden) {
  layer_set_hidden(forecast_layer->layer, hidden);
}