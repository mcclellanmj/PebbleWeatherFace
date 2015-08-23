#include <pebble.h>
#include "current_weather_layer.h"
#include "util.h"

static GBitmap* get_icon_for(const enum WeatherIcon icon) {
  //gbitmap_create_as_sub_bitmap()
  return NULL;
// TODO switch statement
}

static void draw_weather_icon(const CurrentWeatherLayer* weather_layer, GContext* ctx) {
  graphics_draw_bitmap_in_rect(ctx, get_icon_for(weather_layer->weather.icon), GRect(0, 0, 50, 50));
}

static void draw_weather(Layer* layer, GContext* ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing the weather");
}

CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, WeatherShort current_weather) {
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
  layer_destroy(current_weather_layer->icon_layer):
  layer_destroy(current_weather_layer->layer);
}

void current_weather_layer_set_foreground_color(CurrentWeatherLayer* current_weather_layer, GColor foreground_color) {
  current_weather_layer->foreground_color = foreground_color;
}

void current_weather_layer_set_weather_state(CurrentWeatherLayer* current_weather_layer,
                                               WeatherShort current_weather) {

}
