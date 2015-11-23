#include <pebble.h>
#include "copying_text_layer.h"
#include "util.h"

CopyingTextLayer* copying_text_layer_create(GRect frame, const char* text, size_t max) {
  CopyingTextLayer *copying_text_layer = (CopyingTextLayer *) malloc(sizeof(CopyingTextLayer));
  *copying_text_layer = (CopyingTextLayer) {
    .text_layer = text_layer_create(frame),
    .text = copy_string(text, max),
    .max_size = max,
  };

  text_layer_set_text(copying_text_layer->text_layer, copying_text_layer->text);
  text_layer_set_background_color(copying_text_layer->text_layer, GColorClear);
  text_layer_set_text_color(copying_text_layer->text_layer, GColorWhite);

  return copying_text_layer;
}

Layer* copying_text_layer_get_layer(CopyingTextLayer *copying_text_layer) {
  return text_layer_get_layer(copying_text_layer->text_layer);
}

void copying_text_layer_set_overflow(CopyingTextLayer *copying_text_layer, GTextOverflowMode overflow_mode) {
  text_layer_set_overflow_mode(copying_text_layer->text_layer, overflow_mode);
}

void copying_text_layer_set_text_alignment(CopyingTextLayer *copying_text_layer, GTextAlignment text_alignment) {
  text_layer_set_text_alignment(copying_text_layer->text_layer, text_alignment);
}

void copying_text_layer_set_font(CopyingTextLayer *copying_text_layer, GFont font) {
  text_layer_set_font(copying_text_layer->text_layer, font);
}

void copying_text_layer_set_text(CopyingTextLayer *copying_text_layer, const char* text) {
  char* new_text = copy_string(text, copying_text_layer->max_size);
  text_layer_set_text(copying_text_layer->text_layer, new_text);
  free(copying_text_layer->text);
  copying_text_layer->text = new_text;
}

void copying_text_layer_set_background_color(CopyingTextLayer *copying_text_layer, GColor background_color) {
  text_layer_set_background_color(copying_text_layer->text_layer, background_color);
}

void copying_text_layer_set_text_color(CopyingTextLayer *copying_text_layer, GColor foreground_color) {
  text_layer_set_text_color(copying_text_layer->text_layer, foreground_color);
}

void copying_text_layer_destroy(CopyingTextLayer *copying_text_layer) {
  text_layer_destroy(copying_text_layer->text_layer);
  free(copying_text_layer->text);
  free(copying_text_layer);
}