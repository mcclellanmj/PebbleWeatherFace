#pragma once
#include <pebble.h>
  
typedef struct {
  uint32_t current_id;
  GBitmap* current_bitmap;
} BitmapContainer;

BitmapContainer* bitmap_container_create();
void bitmap_container_load(BitmapContainer *container, uint32_t resource_id);
GBitmap* bitmap_container_get_current(BitmapContainer *container);
void bitmap_container_destroy(BitmapContainer *container);