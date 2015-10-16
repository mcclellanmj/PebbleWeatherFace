#include <pebble.h>
#include "current_weather_layer.h"
#include "bitmap_container.h"
#include "util.h"

enum WeatherFonts { TEMPERATURE };

const uint32_t OFFSET_TO_RESOURCE_MAPPING[25] = { RESOURCE_ID_WEATHER_ICON_0, RESOURCE_ID_WEATHER_ICON_1, RESOURCE_ID_WEATHER_ICON_2, RESOURCE_ID_WEATHER_ICON_3, RESOURCE_ID_WEATHER_ICON_4, RESOURCE_ID_WEATHER_ICON_5, RESOURCE_ID_WEATHER_ICON_6, RESOURCE_ID_WEATHER_ICON_7, RESOURCE_ID_WEATHER_ICON_8, RESOURCE_ID_WEATHER_ICON_9, RESOURCE_ID_WEATHER_ICON_10, RESOURCE_ID_WEATHER_ICON_11, RESOURCE_ID_WEATHER_ICON_12, RESOURCE_ID_WEATHER_ICON_13, RESOURCE_ID_WEATHER_ICON_14, RESOURCE_ID_WEATHER_ICON_15, RESOURCE_ID_WEATHER_ICON_16, RESOURCE_ID_WEATHER_ICON_17, RESOURCE_ID_WEATHER_ICON_18, RESOURCE_ID_WEATHER_ICON_19, RESOURCE_ID_WEATHER_ICON_20, RESOURCE_ID_WEATHER_ICON_21, RESOURCE_ID_WEATHER_ICON_22, RESOURCE_ID_WEATHER_ICON_23, RESOURCE_ID_WEATHER_ICON_24 };

static const int8_t BITMAP_SIZE = 45;

CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, CurrentWeather current_weather) {
  Layer* layer = layer_create_with_data(frame, sizeof(CurrentWeatherLayer));
  CurrentWeatherLayer* weather_layer = layer_get_data(layer);

  *weather_layer = (CurrentWeatherLayer) {
    .layer = layer,
    .icon_layer = bitmap_layer_create(GRect(8, 8, BITMAP_SIZE, BITMAP_SIZE)),
    .temperature_layer = text_layer_create(GRect(8, 50, 45, 35)),
    .background_color = GColorClear,
    .foreground_color = GColorWhite,
    .initialized_state = (LazyLoadState) {
      .bitmap_container = bitmap_container_create()
    }
  };
  
  text_layer_set_text_color(weather_layer->temperature_layer, GColorWhite);
  text_layer_set_background_color(weather_layer->temperature_layer, GColorBlack);
  text_layer_set_font(weather_layer->temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(weather_layer->temperature_layer, GTextAlignmentCenter);
  
  layer_add_child(layer, text_layer_get_layer(weather_layer->temperature_layer));
  layer_add_child(layer, bitmap_layer_get_layer(weather_layer->icon_layer));
  
  current_weather_layer_set_weather(weather_layer, current_weather);

  return weather_layer;
}

Layer* current_weather_layer_get_layer(CurrentWeatherLayer *current_weather_layer) {
  return current_weather_layer->layer;
}

void current_weather_layer_destroy(CurrentWeatherLayer* current_weather_layer) {
  bitmap_container_destroy(current_weather_layer->initialized_state.bitmap_container);
  bitmap_layer_destroy(current_weather_layer->icon_layer);
  text_layer_destroy(current_weather_layer->temperature_layer);
  
  layer_destroy(current_weather_layer->layer);
}

void current_weather_layer_set_foreground_color(CurrentWeatherLayer* current_weather_layer, GColor foreground_color) {
  current_weather_layer->foreground_color = foreground_color;
}

void current_weather_layer_set_weather(CurrentWeatherLayer* current_weather_layer,
                                               CurrentWeather current_weather) {
  // Setup the bitmap layer
  bitmap_container_load(current_weather_layer->initialized_state.bitmap_container, OFFSET_TO_RESOURCE_MAPPING[current_weather.icon_offset]);
  bitmap_layer_set_bitmap(current_weather_layer->icon_layer, bitmap_container_get_current(current_weather_layer->initialized_state.bitmap_container));
  layer_mark_dirty(bitmap_layer_get_layer(current_weather_layer->icon_layer));
  
  // Setup the temperature layer
  // Has to be static because managing it manually is more work than this little hack
  static char temperature_buffer[7];
  snprintf(temperature_buffer, 7, "%d\u00B0", current_weather.temperature);
  text_layer_set_text(current_weather_layer->temperature_layer, temperature_buffer);
  layer_mark_dirty(text_layer_get_layer(current_weather_layer->temperature_layer));
}