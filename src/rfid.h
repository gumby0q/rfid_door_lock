#include <Arduino.h>

#include "configs.h"
#define KEYS_CACHE_SIZE ((uint8_t)256)

void rfid_init(void);
int8_t rfid_scan(String * id);
int8_t rfid_validate_id(String id, int8_t * validation_status);
void rfid_cache_id(String id);
