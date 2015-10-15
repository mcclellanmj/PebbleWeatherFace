#include <pebble.h>
#include "current_weather_layer.h"
#include "bitmap_container.h"
#include "util.h"

enum WeatherFonts { TEMPERATURE };

const uint32_t OFFSET_TO_RESOURCE_MAPPING[25] = { RESOURCE_ID_WEATHER_ICON_0, RESOURCE_ID_WEATHER_ICON_1, RESOURCE_ID_WEATHER_ICON_2, RESOURCE_ID_WEATHER_ICON_3, RESOURCE_ID_WEATHER_ICON_4, RESOURCE_ID_WEATHER_ICON_5, RESOURCE_ID_WEATHER_ICON_6, RESOURCE_ID_WEATHER_ICON_7, RESOURCE_ID_WEATHER_ICON_8, RESOURCE_ID_WEATHER_ICON_9, RESOURCE_ID_WEATHER_ICON_10, RESOURCE_ID_WEATHER_ICON_11, RESOURCE_ID_WEATHER_ICON_12, RESOURCE_ID_WEATHER_ICON_13, RESOURCE_ID_WEATHER_ICON_14, RESOURCE_ID_WEATHER_ICON_15, RESOURCE_ID_WEATHER_ICON_16, RESOURCE_ID_WEATHER_ICON_17, RESOURCE_ID_WEATHER_ICON_18, RESOURCE_ID_WEATHER_ICON_19, RESOURCE_ID_WEATHER_ICON_20, RESOURCE_ID_WEATHER_ICON_21, RESOURCE_ID_WEATHER_ICON_22, RESOURCE_ID_WEATHER_ICON_23, RESOURCE_ID_WEATHER_ICON_24 };

static const int8_t BITMAP_SIZE = 45;

static inline GFont font_for(const enum WeatherFonts fonts) {
  switch(fonts) {
    case TEMPERATURE:
      return fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    default:
      // TODO: Needs to create an error message and stop
      return NULL;
  }
}

static void draw_weather_icon(const CurrentWeatherLayer* weather_layer, GContext* ctx) {
  const GRect position = GRect(8, 8, BITMAP_SIZE, BITMAP_SIZE);
  bitmap_container_load(weather_layer->initialized_state.bitmap_container, OFFSET_TO_RESOURCE_MAPPING[weather_layer->current_weather.icon_offset]);
  
  graphics_draw_bitmap_in_rect(
          ctx,
          bitmap_container_get_current(weather_layer->initialized_state.bitmap_container),
          position);
}

static void draw_temperature(char* temperature, GContext* ctx) {
  graphics_draw_text( ctx
                    , temperature
                    , font_for(TEMPERATURE)
                    , GRect(8, 50, 45, 35)
                    , GTextOverflowModeFill
                    , GTextAlignmentCenter
                    , NULL);

}

// TODO: These layers should be split into sub layers to prevent too much work
static void draw_weather(Layer* layer, GContext* ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing the weather");
  CurrentWeatherLayer* weather_layer = layer_get_data(layer);

  char temperature[7];
  snprintf(temperature, 7, "%d\u00B0", weather_layer->current_weather.temperature);
  draw_temperature(temperature, ctx);

  draw_weather_icon(weather_layer, ctx);
}

CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, CurrentWeather current_weather) {
  Layer* layer = layer_create_with_data(frame, sizeof(CurrentWeatherLayer));
  CurrentWeatherLayer* weather_layer = layer_get_data(layer);

  *weather_layer = (CurrentWeatherLayer) {
    .layer = layer,
    .background_color = GColorClear,
    .foreground_color = GColorWhite,
    .initialized_state = (LazyLoadState) {
      .bitmap_container = bitmap_container_create()
    }
  };
  
  current_weather_layer_set_weather(weather_layer, current_weather);

  layer_set_update_proc(layer, draw_weather);
  return weather_layer;
}

Layer* current_weather_layer_get_layer(CurrentWeatherLayer *current_weather_layer) {
  return current_weather_layer->layer;
}

void current_weather_layer_destroy(CurrentWeatherLayer* current_weather_layer) {
  bitmap_container_destroy(current_weather_layer->initialized_state.bitmap_container);
  layer_destroy(current_weather_layer->layer);
}

void current_weather_layer_set_foreground_color(CurrentWeatherLayer* current_weather_layer, GColor foreground_color) {
  current_weather_layer->foreground_color = foreground_color;
}

void current_weather_layer_set_weather(CurrentWeatherLayer* current_weather_layer,
                                               CurrentWeather current_weather) {
  current_weather_layer->current_weather = current_weather;
  
  layer_mark_dirty(current_weather_layer->layer);
}