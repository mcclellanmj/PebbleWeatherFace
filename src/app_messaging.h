#pragma once
#include <pebble.h>
  
typedef void (*RequestTypeHandler)(DictionaryIterator *iter, void *context);

void messaging_init();
void register_handler(uint8_t request_type, RequestTypeHandler* handler);
void handle_message();