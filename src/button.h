#include <Arduino.h>

#include "configs.h"
#define DEBOUNCE_TIME_MS ((uint16_t)40)

void button_init(void);
void button_attach_handler(void (*f)());
