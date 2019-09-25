#include <Arduino.h>
#include "configs.h"

/* errors */
#define HTTP_POST_ERROR         ((int8_t)-1)
#define HTTP_UNABLE_TO_CONNECT  ((int8_t)-2)
#define WIFI_UNABLE_TO_CONNECT  ((int8_t)-3)

void wifi_init(void);
void wifi_loop(void);

int8_t server_validation(String inputId, int8_t* access_status);


void rest_server_init(void);
void rest_server_handle_requests(void);

void rest_server_attach_handler_open(void (*f)());
void rest_server_attach_handler_rfid_reset(void (*f)());


int8_t http_log(String message);

void test_server_validation(void);
