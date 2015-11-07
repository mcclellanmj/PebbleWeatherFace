#include <pebble.h>
#include "current_details_layer.h"

static void create_icon_text_layers(IconTextLayer** icon_text_layers, CurrentWeather current_weather) {
  *icon_text_layers = NULL;
}

CurrentDetailsLayer* current_details_layer_create_layer(GRect frame, CurrentWeather current_weather) {
  Layer *root_layer = layer_create_with_data(frame, sizeof(CurrentDetailsLayer));
  CurrentDetailsLayer *current_details_layer = (CurrentDetailsLayer *) layer_get_data(root_layer);
  *current_details_layer = (CurrentDetailsLayer) {
    .root_layer = root_layer,
  };
  
  create_icon_text_layers(current_details_layer->icon_text_layers, current_weather);
  return current_details_layer;
}

void current_details_layer_destroy(CurrentDetailsLayer *current_details_layer) {
  for(int i = 0; i < 3; i++) {
    icon_text_layer_destroy(current_details_layer->icon_text_layers[i]);
  } 
  
  layer_destroy(current_details_layer->root_layer);
}

void current_details_layer_set_foreground_color(CurrentDetailsLayer *current_details_layer, GColor foreground_color) {
  
}

void current_details_layer_set_background_color(CurrentDetailsLayer *current_details_layer, GColor background_color) {
  
}

void current_details_layer_set_weather(CurrentDetailsLayer *current_details_layer, CurrentWeather current_weather) {
  
}

Layer* current_details_layer_get_layer(CurrentDetailsLayer *current_details_layer) {
  return NULL;
}