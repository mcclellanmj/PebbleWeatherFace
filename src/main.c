#include <pebble.h>
#include "battery.h"
#include "bluetooth_layer.h"
#include "current_weather_layer.h"
#include "forecast_layer.h"
  
enum {
  WEATHER_STATUS = 0,
  WEATHER_TEMP = 1,
  WEATHER_WIND_DIRECTION = 2,
  WEATHER_WIND_SPEED = 3,
  WEATHER_ICON_OFFSET = 4,
  WEATHER_PRECIP = 5,
  WEATHER_FORECAST_TEMPS = 6 ,
  WEATHER_FORECAST_PRECIP_CHANCE = 7,
  KEY_MESSAGE_TYPE = 8,
  WEATHER_FORECAST_START = 9
};

enum {
  PHONE_READY = 0,
  WEATHER_REPORT = 1,
  FETCH_WEATHER = 2,
  WEATHER_FAILED = 3
};

struct Parts {
  Window *main_window;
  TextLayer *time_layer;
  TextLayer *date_layer;
  BatteryLayer *battery_layer;
  BluetoothLayer *bluetooth_layer;
  CurrentWeatherLayer *current_weather_layer;
  ForecastLayer *forecast_layer;
};

typedef struct {
  char *time_text;
  char *date_text;
} TimeInfo;
  
const VibePattern DISCONNECT_PATTERN = {
  .durations = (uint32_t[1]) {2000},
  .num_segments = 1,
};

const VibePattern RECONNECT_PATTERN = {
  .durations = (uint32_t[1]) {100},
  .num_segments = 1,
};

struct Parts *parts;

static TimeInfo get_current_time(const struct tm *tick_time) {
  static TimeInfo time_info;
  
  // Generate the time
  static char time_buffer[6];
  strftime(time_buffer, 6, "%H:%M", tick_time);
  time_info.time_text = time_buffer;
  
  // Generate the date
  static char date_buffer[16];
  strftime(date_buffer, 16, "%A %m/%d", tick_time);
  time_info.date_text = date_buffer;
  
  return time_info;
}

static void update_time(struct Parts *parts, const struct tm *tick_time) {
  TimeInfo time_info = get_current_time(tick_time);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick time is [%s]", time_info.time_text);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Used heap is [%d]", heap_bytes_used());
  
  // Set the new times
  text_layer_set_text(parts->time_layer, time_info.time_text);
  text_layer_set_text(parts->date_layer, time_info.date_text);
  
  // Mark layers as dirty
  layer_mark_dirty(text_layer_get_layer(parts->time_layer));
  layer_mark_dirty(text_layer_get_layer(parts->date_layer));
}

static bool send_request() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending the Fetch Weather command to the phone");
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "null iter");
    return false;
  }

  Tuplet tuple = TupletInteger(KEY_MESSAGE_TYPE, FETCH_WEATHER);
  dict_write_tuplet(iter, &tuple);
  dict_write_end(iter);

  app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Finished the send");
  return true;
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(parts, tick_time);
  if(tick_time->tm_min % 30 == 0) {
    send_request();
  }
}

static Weather initial_weather() {
  Weather the_weather;
  the_weather.current_weather.temperature = -85;
  the_weather.current_weather.wind_speed = 15;
  the_weather.current_weather.wind_dir = 250;
  the_weather.current_weather.icon_offset = 0;

  return the_weather;
}

static Forecast initial_forecast() {
  return (Forecast) {
     .valid = false
  };
}

static Forecast test_forecast() {
  return (Forecast) {
          .valid = true,
          .chance_of_rain = {10, 12, 100, 10, 12, 43, 85, 19, 20, 30, 11, 85},
          .start_time = 12,
          .temperatures = {60, 62, 65, 64, 62, 68, 70, 75, 72, 68, 67, 67}
  };
}

static ForecastLayer* create_forecast_layer() {
  ForecastLayer *forecast_layer = forecast_layer_create_layer(GRect(0, 44, 144, 128), test_forecast());
  forecast_layer_set_hidden(forecast_layer, false);

  return forecast_layer;
}

static CurrentWeatherLayer* create_current_weather_layer() {
  return current_weather_layer_create_layer(GRect(0, 63, 144, 125), initial_weather());
}

static BluetoothLayer* create_bluetooth_layer() {
  bool connected = bluetooth_connection_service_peek();
  BluetoothLayer *bluetooth_layer = bluetooth_layer_create_layer(GRect(122, 29, 17, 13), connected);
  return bluetooth_layer;
}

static BatteryLayer* create_battery_layer() {
  BatteryLayer *battery_layer = battery_layer_create_layer(GRect(120, 12, 20, 13));
  battery_layer->battery_state = battery_state_service_peek();
  return battery_layer;
}

static TextLayer* create_time_layer() {
  TextLayer *text_layer = text_layer_create(GRect(4, 0, 144, 43));
  layer_set_clips(text_layer_get_layer(text_layer), false);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorClear);
  return text_layer;
}

static TextLayer* create_date_layer() {
  TextLayer *date_layer = text_layer_create(GRect(0, 40, 144, 18));
  layer_set_clips(text_layer_get_layer(date_layer), false);
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_background_color(date_layer, GColorClear);
  return date_layer;
}

static void window_load(Window *window) {
  window_set_background_color(window, GColorBlack);
  
  Layer *window_layer = window_get_root_layer(parts->main_window);

  layer_add_child(window_layer, battery_layer_get_layer(parts->battery_layer));
  layer_add_child(window_layer, bluetooth_layer_get_layer(parts->bluetooth_layer));
  layer_add_child(window_layer, text_layer_get_layer(parts->time_layer));
  layer_add_child(window_layer, text_layer_get_layer(parts->date_layer));
  layer_add_child(window_layer, current_weather_layer_get_layer(parts->current_weather_layer));
  layer_add_child(window_layer, forecast_layer_get_layer(parts->forecast_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(parts->time_layer);
  text_layer_destroy(parts->date_layer);
  battery_layer_destroy(parts->battery_layer);
  bluetooth_layer_destroy(parts->bluetooth_layer);
  current_weather_layer_destroy(parts->current_weather_layer);
  forecast_layer_destroy(parts->forecast_layer);
}

static void handle_battery_change(BatteryChargeState charge_state) {
  battery_layer_set_battery_state(parts->battery_layer, charge_state);
}

inline VibePattern get_vibe_pattern(bool connected) {
  if(!connected) {
    return DISCONNECT_PATTERN;
  }

  return RECONNECT_PATTERN;
}

static void handle_bluetooth_change(bool connected) {
  bluetooth_layer_set_bluetooth_state(parts->bluetooth_layer, connected);
  vibes_enqueue_custom_pattern(get_vibe_pattern(connected));
}

static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
  Tuple *init_tuple = dict_find(iterator, KEY_MESSAGE_TYPE);
  uint8_t request_type = init_tuple->value->uint8;
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Got a message with request type [%d]", request_type);
  
  if(request_type == PHONE_READY) {
    send_request();
  }
  
  if(request_type == WEATHER_REPORT) {
    // FIXME: This is prototype code, clean it up!
    int16_t weather_temperature = dict_find(iterator, WEATHER_TEMP)->value->int16;
    int16_t weather_wind_speed = dict_find(iterator, WEATHER_WIND_SPEED)->value->int16;
    int16_t weather_wind_direction = dict_find(iterator, WEATHER_WIND_DIRECTION)->value->int16;
    int8_t icon_offset = dict_find(iterator, WEATHER_ICON_OFFSET)->value->uint8;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got Weather with temp [%d], wind speed [%d], wind direction [%d], icon offset [%d]", weather_temperature, weather_wind_speed, weather_wind_direction, icon_offset);
    Weather weather = current_weather_layer_get_weather(parts->current_weather_layer);
    CurrentWeather *current_weather = &weather.current_weather;
    current_weather->temperature = weather_temperature;
    current_weather->wind_dir = weather_wind_direction;
    current_weather->wind_speed = weather_wind_speed;
    current_weather->icon_offset = icon_offset;
    current_weather_layer_set_weather(parts->current_weather_layer, weather);
    
    // TODO: Parse the response and send the weather over to the draw layer
  }
}

static void handle_init() {
  parts = malloc(sizeof(*parts));
  *parts = (struct Parts) {
    .main_window = window_create(),
    .forecast_layer = create_forecast_layer(),
    .time_layer = create_time_layer(),
    .date_layer = create_date_layer(),
    .battery_layer = create_battery_layer(),
    .bluetooth_layer = create_bluetooth_layer(),
    .current_weather_layer = create_current_weather_layer()
  };
  
  window_set_window_handlers(parts->main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  window_stack_push(parts->main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  battery_state_service_subscribe(handle_battery_change);
  bluetooth_connection_service_subscribe(handle_bluetooth_change);
}

static void handle_deinit() {
  window_destroy(parts->main_window);
  free(parts);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
