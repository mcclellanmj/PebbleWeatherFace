#include <pebble.h>
#include "current_weather_layer.h"
#include "bitmap_container.h"
#include "util.h"

enum WeatherFonts { TEMPERATURE, WIND_SPEED };

const uint32_t OFFSET_TO_RESOURCE_MAPPING[25] = { RESOURCE_ID_WEATHER_ICON_0, RESOURCE_ID_WEATHER_ICON_1, RESOURCE_ID_WEATHER_ICON_2, RESOURCE_ID_WEATHER_ICON_3, RESOURCE_ID_WEATHER_ICON_4, RESOURCE_ID_WEATHER_ICON_5, RESOURCE_ID_WEATHER_ICON_6, RESOURCE_ID_WEATHER_ICON_7, RESOURCE_ID_WEATHER_ICON_8, RESOURCE_ID_WEATHER_ICON_9, RESOURCE_ID_WEATHER_ICON_10, RESOURCE_ID_WEATHER_ICON_11, RESOURCE_ID_WEATHER_ICON_12, RESOURCE_ID_WEATHER_ICON_13, RESOURCE_ID_WEATHER_ICON_14, RESOURCE_ID_WEATHER_ICON_15, RESOURCE_ID_WEATHER_ICON_16, RESOURCE_ID_WEATHER_ICON_17, RESOURCE_ID_WEATHER_ICON_18, RESOURCE_ID_WEATHER_ICON_19, RESOURCE_ID_WEATHER_ICON_20, RESOURCE_ID_WEATHER_ICON_21, RESOURCE_ID_WEATHER_ICON_22, RESOURCE_ID_WEATHER_ICON_23, RESOURCE_ID_WEATHER_ICON_24 };

static const int8_t SUB_BITMAP_SIZE = 45;

static inline GFont font_for(const enum WeatherFonts fonts) {
  switch(fonts) {
    case TEMPERATURE:
      return fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
    case WIND_SPEED:
      return fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    default:
      // TODO: Needs to create an error message and stop
      return NULL;
  }
}

static struct GPathInfo* create_arrow_path() {
  uint32_t size = 7;

  GPoint* points = malloc(sizeof(GPoint) * size);

  points[0] = (GPoint) {0, -8};
  points[1] = (GPoint) {5, -2};
  points[2] = (GPoint) {3, -2};
  points[3] = (GPoint) {3, 8};
  points[4] = (GPoint) {-3, 8};
  points[5] = (GPoint) {-3, -2};
  points[6] = (GPoint) {-5, -2};

  GPathInfo *path_info = malloc(sizeof(GPathInfo));
  *path_info = (GPathInfo) {
          .num_points = size,
          .points = points
  };

  return path_info;
}

static void draw_arrow(GContext* ctx, GPoint origin, uint8_t direction, LazyLoadState state) {
  GPathInfo *path_info = state.arrow_path;

  GPath* path = gpath_create(path_info);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_move_to(path, origin);
  gpath_rotate_to(path, TRIG_MAX_ANGLE / 360 * direction);
  gpath_draw_filled(ctx, path);

  free(path);
}

static void draw_weather_icon(const CurrentWeatherLayer* weather_layer, GContext* ctx) {
  const GRect position = GRect(15, 8, SUB_BITMAP_SIZE, SUB_BITMAP_SIZE);
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
                    , GRect(85, 0, 59, 35)
                    , GTextOverflowModeFill
                    , GTextAlignmentLeft
                    , NULL);

}

static void draw_wind(char* speed, uint8_t direction, GContext* ctx, LazyLoadState state) {
  graphics_draw_text( ctx
                    , speed
                    , font_for(WIND_SPEED)
                    , GRect(85, 35, 30, 30)
                    , GTextOverflowModeFill
                    , GTextAlignmentLeft
                    , NULL);

  draw_arrow(ctx, GPoint(114, 50), direction, state);
}

// TODO: These layers should be split into sub layers to prevent too much work
static void draw_weather(Layer* layer, GContext* ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing the weather");
  CurrentWeatherLayer* weather_layer = layer_get_data(layer);

  char temp[5];
  snprintf(temp, 5, "%d", weather_layer->current_weather.temperature);
  draw_temperature(temp, ctx);

  char speed[4];
  snprintf(speed, 4, "%d", weather_layer->current_weather.wind_speed);
  uint8_t direction = weather_layer->current_weather.wind_dir;
  draw_wind(speed, direction, ctx, weather_layer->initialized_state);

  draw_weather_icon(weather_layer, ctx);
}

CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, CurrentWeather current_weather) {
  Layer* layer = layer_create_with_data(frame, sizeof(CurrentWeatherLayer));
  CurrentWeatherLayer* weather_layer = layer_get_data(layer);

  weather_layer->layer = layer;
  weather_layer->background_color = GColorClear;
  weather_layer->foreground_color = GColorWhite;
  weather_layer->initialized_state.arrow_path = create_arrow_path();
  weather_layer->initialized_state.bitmap_container = bitmap_container_create();
  
  current_weather_layer_set_weather(weather_layer, current_weather);

  layer_set_update_proc(layer, draw_weather);
  return weather_layer;
}

Layer* current_weather_layer_get_layer(CurrentWeatherLayer *current_weather_layer) {
  return current_weather_layer->layer;
}

void current_weather_layer_destroy(CurrentWeatherLayer* current_weather_layer) {
  free(current_weather_layer->initialized_state.arrow_path);
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