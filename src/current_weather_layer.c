#include <pebble.h>
#include "current_weather_layer.h"
#include "util.h"

enum WeatherFonts { TEMPERATURE, WIND_SPEED };

static inline GFont font_for(const enum WeatherFonts fonts) {
  switch(fonts) {
    case TEMPERATURE:
    case WIND_SPEED:
      return FONT_KEY_BITHAM_30_BLACK;
    default:
      return NULL;
  }
}

static GBitmap* get_icon_for(const enum WeatherIcon icon) {
  return NULL;
}

static void draw_arrow(GContext* ctx) {
  uint32_t size = 8;

  GPoint* points = malloc(sizeof(GPoint) * size);
  points[0] = (GPoint) {5, 0};
  points[1] = (GPoint) {10, 5};
  points[2] = (GPoint) {7, 5};
  points[3] = (GPoint) {7, 15};
  points[4] = (GPoint) {3, 15};
  points[5] = (GPoint) {3, 5};
  points[6] = (GPoint) {0, 5};
  points[7] = (GPoint) {5, 0};

  GPathInfo path_info = {
          .num_points = size,
          .points = points
  };

  GPath* path = gpath_create(&path_info);
  gpath_draw_filled(ctx, path);
}

static void draw_weather_icon(const CurrentWeatherLayer* weather_layer, GContext* ctx) {
  graphics_draw_bitmap_in_rect(ctx, get_icon_for(weather_layer->weather.current_weather.icon), GRect(0, 0, 50, 50));
}

static void draw_temperature(char* temperature, GContext* ctx) {
  graphics_draw_text( ctx
                    , temperature
                    , font_for(TEMPERATURE)
                    , GRect(55, 0, 30, 30)
                    , GTextOverflowModeTrailingEllipsis
                    , GTextAlignmentCenter
                    , NULL);
}

static void draw_wind(uint8_t speed, char* direction, GContext* ctx) {
  // TODO: Convert speed to a string
  graphics_draw_text(ctx, "15", font_for(WIND_SPEED), GRect(55, 30, 30, 30), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void draw_weather(Layer* layer, GContext* ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing the weather");
}

CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, Weather current_weather) {
  Layer* layer = layer_create_with_data(frame, sizeof(CurrentWeatherLayer));
  CurrentWeatherLayer* weather_layer = layer_get_data(layer);

  weather_layer->layer = layer;
  weather_layer->background_color = GColorClear;
  weather_layer->foreground_color = GColorWhite;

  layer_set_update_proc(layer, draw_weather);
  return weather_layer;
}

Layer* current_weather_layer_get_layer(CurrentWeatherLayer *current_weather_layer) {
  return current_weather_layer->layer;
}

void current_weather_layer_destroy(CurrentWeatherLayer* current_weather_layer) {
  layer_destroy(current_weather_layer->layer);
}

void current_weather_layer_set_foreground_color(CurrentWeatherLayer* current_weather_layer, GColor foreground_color) {
  current_weather_layer->foreground_color = foreground_color;
}

void current_weather_layer_set_weather(CurrentWeatherLayer* current_weather_layer,
                                               Weather current_weather) {
  current_weather_layer->weather = current_weather;
}
