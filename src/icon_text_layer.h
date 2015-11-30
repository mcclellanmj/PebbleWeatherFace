#pragma once
#include <pebble.h>
#include "copying_text_layer.h"

typedef struct {
  Layer *root_layer;
  BitmapLayer *icon_layer;
  GBitmap *bitmap;
  CopyingTextLayer *text_layer;
  GColor background_color;
  GColor foreground_color;
} IconTextLayer;

IconTextLayer* icon_text_layer_create(const GRect frame, GBitmap *bitmap, const char *text, size_t text_max);
Layer* icon_text_layer_get_layer(const IconTextLayer *icon_text_layer);
void icon_text_layer_set_text(IconTextLayer *icon_text_layer, const char *text);
void icon_text_layer_set_bitmap(IconTextLayer *icon_text_layer, GBitmap *bitmap);
void icon_text_layer_destroy(IconTextLayer *icon_text_layer);