#pragma once
#include <pebble.h>
#include "util.h"

typedef struct {
  TextLayer *text_layer;
  size_t max_size;
  char *text;
} CopyingTextLayer;

CopyingTextLayer* copying_text_layer_create(GRect frame, const char* text, size_t max);
void copying_text_layer_destroy(CopyingTextLayer *copying_text_layer);

void copying_text_layer_set_background_color(CopyingTextLayer *copying_text_layer, GColor background_color);
void copying_text_layer_set_text_color(CopyingTextLayer *copying_text_layer, GColor foreground_color);

Layer* copying_text_layer_get_layer(CopyingTextLayer *copying_text_layer);
void copying_text_layer_set_overflow(CopyingTextLayer *copying_text_layer, GTextOverflowMode overflow_mode);
void copying_text_layer_set_text_alignment(CopyingTextLayer *copying_text_layer, GTextAlignment text_alignment);
void copying_text_layer_set_font(CopyingTextLayer *copying_text_layer, GFont font);
void copying_text_layer_set_text(CopyingTextLayer *copying_text_layer, const char* text);