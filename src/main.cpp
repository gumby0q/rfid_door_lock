
#include "main.h"

#include "rfid.h"
#include "networking.h"
#include "lock_control.h"
#include "button.h"


void main_setup(void) {
    Serial.begin(115200);
    // Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();
    Serial.println();

    lock_control_init();
    button_init();
    button_attach_handler(&open);

    wifi_init();
    rfid_init();

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

        int8_t validation_status = 0;
        rfid_validate_id(id, &validation_status);

        if (validation_status != 0) {
            Serial.println("cache validation_status success");
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
                }
            } else if (error != 0) {
                Serial.println("server_validation error");
                Serial.print("error ");
                Serial.print(error);
                Serial.println(" ");
            }
        }
    }

    delay(500);
}
