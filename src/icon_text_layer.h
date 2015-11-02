#pragma once
#include <pebble.h>

typedef struct {
  Layer *root_layer;
  BitmapLayer *icon_layer;
  char* current_text;
  TextLayer *text_layer;
  GColor background_color;
  GColor foreground_color;
} IconTextLayer;

IconTextLayer* icon_text_layer_create_layer(GRect frame, GBitmap *bitmap, char *text);
Layer* icon_text_layer_get_layer(IconTextLayer *icon_text_layer);
void icon_text_layer_set_text(IconTextLayer *icon_text_layer, char *text);
void icon_text_layer_set_bitmap(IconTextLayer *icon_text_layer, GBitmap *bitmap);
void icon_text_layer_destroy(IconTextLayer *icon_text_layer);