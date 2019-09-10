#include <Arduino.h>
#include "configs.h"

/* errors */
#define HTTP_POST_ERROR         ((int8_t)-1)
#define HTTP_UNABLE_TO_CONNECT  ((int8_t)-2)
#define WIFI_UNABLE_TO_CONNECT  ((int8_t)-3)

void wifi_init(void);
void wifi_loop(void);

int8_t server_validation(String inputId, int8_t* access_status);

void test_server_validation(void);
