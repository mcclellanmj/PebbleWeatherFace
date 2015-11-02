#include <pebble.h>
#include "bitmap_container.h"
  
BitmapContainer* bitmap_container_create() {
  BitmapContainer *container = (BitmapContainer *) malloc(sizeof(BitmapContainer));
  container->current_bitmap = NULL;
  return container;
}

void bitmap_container_load(BitmapContainer *container, uint32_t resource_id) {
  if(resource_id != container->current_id) {
    GBitmap *old_bitmap = container->current_bitmap;
    container->current_id = resource_id;
    container->current_bitmap = gbitmap_create_with_resource(resource_id);
    gbitmap_destroy(old_bitmap);
  }
}

GBitmap* bitmap_container_get_current(BitmapContainer *container) {
  return container->current_bitmap;
}

void bitmap_container_destroy(BitmapContainer *container) {
  if(container->current_bitmap != NULL) {
    gbitmap_destroy(container->current_bitmap);
  }
  free(container);
}