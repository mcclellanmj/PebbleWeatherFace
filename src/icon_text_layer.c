#include <pebble.h>
#include "icon_text_layer.h"
#include "util.h"

static const uint8_t ICON_SIZE_X = 15;
static const uint8_t ICON_SIZE_Y = 15;

static const uint8_t TEXT_SIZE_X = 50;
static const uint8_t TEXT_SIZE_Y = 20;

IconTextLayer* icon_text_layer_create_layer(const GRect frame, const GBitmap *bitmap, const char *text) {
  Layer *layer = layer_create_with_data(frame, sizeof(IconTextLayer));
  IconTextLayer *icon_text_layer = (IconTextLayer *) layer_get_data(layer);
  *icon_text_layer = (IconTextLayer) {
    .background_color = GColorBlack,
    .foreground_color = GColorWhite,
    .root_layer = layer,
    .current_text = copy_string(text, 10),
    .icon_layer = bitmap_layer_create(
                    GRect( frame.origin.x
                         , frame.origin.y
                         , ICON_SIZE_X
                         , ICON_SIZE_Y)
                  ),
    .text_layer = text_layer_create(
                    GRect( frame.origin.x + ICON_SIZE_X + 4
                         , frame.origin.y
                         , TEXT_SIZE_X
                         , TEXT_SIZE_Y )
                  )
  };

  layer_add_child(layer, bitmap_layer_get_layer(icon_text_layer->icon_layer));
  layer_add_child(layer, text_layer_get_layer(icon_text_layer->text_layer));

  return icon_text_layer;
}

Layer* icon_text_layer_get_layer(const IconTextLayer *icon_text_layer) {
  return icon_text_layer->root_layer;
}

void icon_text_layer_set_text(IconTextLayer *icon_text_layer, const char *text) {
  icon_text_layer->current_text = copy_string(text, 10);
  text_layer_set_text(icon_text_layer->text_layer, icon_text_layer->current_text);
  layer_mark_dirty(text_layer_get_layer(icon_text_layer->text_layer));
}

void icon_text_layer_destroy(IconTextLayer *icon_text_layer) {
  bitmap_layer_destroy(icon_text_layer->icon_layer);
  text_layer_destroy(icon_text_layer->text_layer);
  free(icon_text_layer->current_text);
  layer_destroy(icon_text_layer->root_layer);
}