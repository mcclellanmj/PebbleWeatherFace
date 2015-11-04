#include <pebble.h>
#include "icon_text_layer.h"
#include "util.h"
#include "copying_text_layer.h"

static const uint8_t ICON_SIZE_X = 15;
static const uint8_t ICON_SIZE_Y = 15;

static const uint8_t TEXT_SIZE_X = 50;
static const uint8_t TEXT_SIZE_Y = 20;

IconTextLayer* icon_text_layer_create_layer(const GRect frame, const GBitmap *bitmap, const char *text, size_t text_max) {
  Layer *layer = layer_create_with_data(frame, sizeof(IconTextLayer));
  IconTextLayer *icon_text_layer = (IconTextLayer *) layer_get_data(layer);
  *icon_text_layer = (IconTextLayer) {
    .background_color = GColorClear,
    .foreground_color = GColorWhite,
    .root_layer = layer,
    .icon_layer = bitmap_layer_create(
                    GRect( frame.origin.x
                         , frame.origin.y
                         , ICON_SIZE_X
                         , ICON_SIZE_Y)
                  ),
    .text_layer = copying_text_layer_create(
                    GRect( frame.origin.x + ICON_SIZE_X + 4
                         , frame.origin.y
                         , TEXT_SIZE_X
                         , TEXT_SIZE_Y ), text, text_max)
  };

  layer_add_child(layer, bitmap_layer_get_layer(icon_text_layer->icon_layer));
  layer_add_child(layer, copying_text_layer_get_layer(icon_text_layer->text_layer));

  return icon_text_layer;
}

Layer* icon_text_layer_get_layer(const IconTextLayer *icon_text_layer) {
  return icon_text_layer->root_layer;
}

void icon_text_layer_set_text(IconTextLayer *icon_text_layer, const char *text) {
  copying_text_layer_set_text(icon_text_layer->text_layer, text);
  layer_mark_dirty(copying_text_layer_get_layer(icon_text_layer->text_layer));
}

void icon_text_layer_destroy(IconTextLayer *icon_text_layer) {
  bitmap_layer_destroy(icon_text_layer->icon_layer);
  copying_text_layer_destroy(icon_text_layer->text_layer);
  layer_destroy(icon_text_layer->root_layer);
  
  free(icon_text_layer);
}