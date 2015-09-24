#include <pebble.h>
#include "current_weather_layer.h"
#include "util.h"

enum WeatherFonts { TEMPERATURE, WIND_SPEED };
static const uint8_t SUB_BITMAP_SIZE = 60;

static inline GFont font_for(const enum WeatherFonts fonts) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Getting the font");
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

static GBitmap* get_icon_for(const enum WeatherIcon icon, const GBitmap* parent_bitmap) {
    switch(icon) {
        case GOOD:
            return gbitmap_create_as_sub_bitmap(parent_bitmap, GRect(0, 0, SUB_BITMAP_SIZE, SUB_BITMAP_SIZE));
        default:
            return gbitmap_create_as_sub_bitmap(parent_bitmap, GRect(SUB_BITMAP_SIZE, 0, SUB_BITMAP_SIZE, SUB_BITMAP_SIZE));
    }

    // TODO: Needs to create an error message and stop the app here
    return NULL;
}

static void draw_arrow(GContext* ctx, GPoint origin, uint8_t direction) {
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

  GPath* path = gpath_create(path_info);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_move_to(path, origin);
  gpath_rotate_to(path, TRIG_MAX_ANGLE / 360 * 180);
  gpath_draw_filled(ctx, path);
}

static void draw_weather_icon(const CurrentWeatherLayer* weather_layer, GContext* ctx) {
  const GRect position = GRect(15, 10, SUB_BITMAP_SIZE, SUB_BITMAP_SIZE);

  graphics_draw_bitmap_in_rect(
          ctx,
          get_icon_for(weather_layer->weather.current_weather.icon, weather_layer->icons_bitmap),
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

static void draw_wind(char* speed, uint8_t direction, GContext* ctx) {
  graphics_draw_text( ctx
                    , speed
                    , font_for(WIND_SPEED)
                    , GRect(85, 35, 30, 30)
                    , GTextOverflowModeFill
                    , GTextAlignmentLeft
                    , NULL);

  draw_arrow(ctx, GPoint(114, 50), direction);
}

// TODO: These layers should be split into sub layers to prevent too much work
static void draw_weather(Layer* layer, GContext* ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing the weather");
  CurrentWeatherLayer* weather_layer = layer_get_data(layer);

  char temp[5];
  snprintf(temp, 5, "%d", weather_layer->weather.current_weather.temperature);
  draw_temperature(temp, ctx);

  char speed[4];
  snprintf(speed, 4, "%d", weather_layer->weather.current_weather.wind_speed);
  uint8_t direction = weather_layer->weather.current_weather.wind_dir;
  draw_wind(speed, direction, ctx);

  draw_weather_icon(weather_layer, ctx);
}

CurrentWeatherLayer* current_weather_layer_create_layer(GRect frame, Weather current_weather) {
  Layer* layer = layer_create_with_data(frame, sizeof(CurrentWeatherLayer));
  CurrentWeatherLayer* weather_layer = layer_get_data(layer);

  weather_layer->layer = layer;
  weather_layer->icons_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICONS_60X60);
  weather_layer->background_color = GColorClear;
  weather_layer->foreground_color = GColorWhite;
  weather_layer->weather = current_weather;

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
