#include "networking.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

void wifi_init(void)
{
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(WIFI_NAME, WIFI_PASS);
}

/* just connection checking */
void wifi_loop(void)
{
    WiFiMulti.run();
}

int8_t server_validation(String inputId, int8_t* access_status)
{
    int8_t error = 0;
    // wait for WiFi connection
    if ((WiFiMulti.run() == WL_CONNECTED)) {

        WiFiClient client;

        HTTPClient http;

        Serial.print("HTTP begin...\n");
        
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["event"] = "check";
        root["cardid"] = inputId.c_str();

        char JSONmessageBuffer[300];
        root.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        // Serial.println(JSONmessageBuffer);
    
        if (http.begin(client, SERVER_ADDRESS)) {  // HTTP
            http.addHeader("Content-Type", "application/json");  //Specify content-type header
            int httpCode = http.POST(JSONmessageBuffer);   //Send the request
            Serial.print("httpCode ");
            Serial.println(httpCode);   //Print HTTP return code

            /* if respose SUCCESS 200 */
            if (httpCode == 200) {
                String response = http.getString();
                Serial.println("success server response: '" + response + "'");
                
                char json[] = "";
                response.toCharArray(json,200);
                JsonObject& root = jsonBuffer.parseObject(json);
                bool needopen = root["access"];

                if (needopen) {
                    Serial.println("access YES");
                    
                    /* access allowed */
                    *access_status = (int8_t)-1; /* true */
                } else {
                    Serial.println("access NO");
                    
                    /* access denied */
                    *access_status = (int8_t)0; /* false */
                }
            } else  if (httpCode < 0) {
                error = HTTP_POST_ERROR;
            }

            http.end();  //Close connection
        } else {
            Serial.printf("HTTP Unable to connect\n");
            error = HTTP_UNABLE_TO_CONNECT;
        }
    } else {
        Serial.println("***wifi connecting failed***");    //Print request response payload
        error = WIFI_UNABLE_TO_CONNECT;
    }

    return error;
}

void test_server_validation(void) {
    String testId = "64a1ccbe";
    int8_t return_status;
    int8_t error;
    error = server_validation(testId, &return_status);

    if (error == 0) {
        Serial.println("test_server_validation pass");
        Serial.print("return_status ");
        Serial.print(return_status);
        Serial.println(" ");
    } else if (error != 0) {
        Serial.println("test validation failed");
        Serial.print("error ");
        Serial.print(error);
        Serial.println(" ");
    }
}
