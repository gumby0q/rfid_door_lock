
#include "main.h"

#include "rfid.h"
#include "networking.h"
#include "lock_control.h"
#include "status_leds.h"
#include "button.h"


#define LOOPCOUNTER_MAX (uint16_t)(8*60)
#define LOOP_DELAY (uint16_t)(125)
uint16_t loopCounter = 0;

int8_t last_rfid_error = 0;

void main_setup(void) {
    Serial.begin(115200);
    // Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();
    Serial.println();

    lock_control_init();
    #ifdef KEY_REGISTRATION_DEVICE
        status_leds_init();
    #endif
    button_init();
    button_attach_handler(&open);

    wifi_init();
    rfid_init();
    
    rest_server_init();

    rest_server_attach_handler_open(&open);
    rest_server_attach_handler_rfid_reset(&rfid_reinit);

    Serial.println("SETUP END");
}

void main_loop(void) {
    int8_t error = 0;
    String id;
    error = rfid_scan(&id);

    if (error == 0) {
        Serial.print("main id");
        Serial.print(id);
        Serial.println(" ");

        /* http log >>> */
        int8_t http_log_error = 0;
        // http_log_error = http_log("rfid_scan ok ");
        // if (http_log_error != 0) {
        //     Serial.print("http_log_error rfid_scan ok ");
        //     Serial.print(http_log_error);
        //     Serial.println(" ");
        // }
        /* http log <<< */

        int8_t validation_status = 0;

        #ifndef KEY_REGISTRATION_DEVICE
        rfid_validate_id(id, &validation_status);
        #endif

        if (validation_status != 0) {
            Serial.println("cache validation_status success");
            open();

            /* http log >>> */
            int8_t http_log_error = 0;
            http_log_error = http_log("cache open ");
            if (http_log_error != 0) {
                Serial.print("http_log_error cache open ");
                Serial.print(http_log_error);
                Serial.println(" ");
            }
            /* http log <<< */

        } else {
            Serial.println("cache validation_status fail");

            int8_t return_status;
            int8_t error;
            error = server_validation(id, &return_status);

            if (error == 0) {
                // Serial.print("return_status ");
                // Serial.print(return_status);
                // Serial.println(" ");
                if (return_status != 0) { /* true */
                    Serial.println("server_validation pass");
                    
                    rfid_cache_id(id);
                    open();
                } else { /* false */
                    Serial.println("server_validation fail");

                    #ifdef KEY_REGISTRATION_DEVICE
                        status_leds_blink_red();
                    #endif
                }
            } else if (error != 0) {
                #ifdef KEY_REGISTRATION_DEVICE
                if (error == WIFI_UNABLE_TO_CONNECT) {
                    open();
                    status_leds_blink_red();
                }
                #endif

                Serial.println("server_validation error");
                Serial.print("error ");
                Serial.print(error);
                Serial.println(" ");
            }

            /* http log >>> */
            int8_t http_log_error = 0;
            http_log_error = http_log("server val err:" + String(error) + " ret stat:" +String(return_status));
            if (http_log_error != 0) {
                Serial.print("http_log_error server val err ");
                Serial.print(http_log_error);
                Serial.println(" ");
            }
            /* http log <<< */
        }

        last_rfid_error = 0;


        // loopCounter = LOOPCOUNTER_MAX-1;
    } else {
        if (error != last_rfid_error) {
            last_rfid_error = error;
            Serial.println("rfid_scan error");
            Serial.print("error ");
            Serial.print(error);
            Serial.println(" ");

            /* http log >>> */
            int8_t http_log_error = 0;
            http_log_error = http_log("rfid_scan error " + String(error));
            if (http_log_error != 0) {
                Serial.print("http_log_error rfid_scan ");
                Serial.print(http_log_error);
                Serial.println(" ");
            }
            /* http log <<< */
        }
    }

    delay(LOOP_DELAY);

    rest_server_handle_requests();

    /* loop log */
    loopCounter++;
    if (loopCounter >= LOOPCOUNTER_MAX) {
        loopCounter = 0;

        /* http log >>> */
        int8_t http_log_error = 0;
        http_log_error = http_log("loop log");
        if (http_log_error != 0) {
            Serial.print("http_log_error loop log ");
            Serial.print(http_log_error);
            Serial.println(" ");
        }
        /* http log <<< */
    }
}
