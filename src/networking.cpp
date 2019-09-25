#include "networking.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ESP8266WebServer.h>
ESP8266WebServer http_rest_server(HTTP_REST_PORT);
typedef void (*request_handler)(void);
request_handler server_handler_open;
request_handler server_handler_rfid_reset;

ESP8266WiFiMulti WiFiMulti;

void post_open(void);
void post_rfid_reset(void);
void config_rest_server_routing(void);

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
            } else  if (httpCode != 200) {
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



int8_t http_log(String message)
{
    int8_t error = 0;

#ifdef SERVER_LOGS
    // wait for WiFi connection
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

        WiFiClient client;
        HTTPClient http;

        Serial.print("HTTP log begin...\n");
        
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["message"] = message;
        // root["cardid"] = inputId.c_str();

        char JSONmessageBuffer[300];
        root.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        // Serial.println(JSONmessageBuffer);
    
        if (http.begin(client, LOG_SERVER_ADDRESS)) {  // HTTP
            http.addHeader("Content-Type", "application/json");  //Specify content-type header
            int httpCode = http.POST(JSONmessageBuffer);   //Send the request
            Serial.print("httpCode ");
            Serial.println(httpCode);   //Print HTTP return code

            /* if respose SUCCESS 200 */
            if (httpCode == 200) {
                // String response = http.getString();
                // Serial.println("success server response: '" + response + "'");
                
                // char json[] = "";
                // response.toCharArray(json,200);
                // JsonObject& root = jsonBuffer.parseObject(json);
                // bool needopen = root["access"];

                // if (needopen) {
                //     Serial.println("access YES");
                    
                //     /* access allowed */
                //     *access_status = (int8_t)-1; /* true */
                // } else {
                //     Serial.println("access NO");
                    
                //     /* access denied */
                //     *access_status = (int8_t)0; /* false */
                // }
            } else  if (httpCode != 200) {
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
#endif SERVER_LOGS

    return error;
}



/* ************************************************************* */
/* ************************************************************* */
/* ************************************************************* */
/* *********************** SERVER ****************************** */
/* ************************************************************* */
/* ************************************************************* */
void post_open(void)
{
    StaticJsonBuffer<500> jsonBuffer;
    String post_body = http_rest_server.arg("plain");
    Serial.println(post_body);

    JsonObject& jsonBody = jsonBuffer.parseObject(http_rest_server.arg("plain"));

    Serial.print("HTTP Method: ");
    Serial.println(http_rest_server.method());
    
    if (!jsonBody.success()) {
        Serial.println("error in parsing json body");
        http_rest_server.send(400);
    }
    else {   
        if (http_rest_server.method() == HTTP_POST) {
            if (jsonBody["id"] == 8) {
                if (server_handler_open != NULL) {
                    server_handler_open();
                }
                // json_to_resource(jsonBody);
                http_rest_server.sendHeader("Status", String("done"));
                http_rest_server.send(201);
                // pinMode(led_resource.gpio, OUTPUT);
            // }
            // else if (jsonBody["id"] == 0)
            //   http_rest_server.send(404);
            // else if (jsonBody["id"] == 2)
            //   http_rest_server.send(409);
            } else {
              http_rest_server.send(409);
            }
        }
    }
}


void post_rfid_reset(void)
{
    StaticJsonBuffer<500> jsonBuffer;
    String post_body = http_rest_server.arg("plain");
    Serial.println(post_body);

    JsonObject& jsonBody = jsonBuffer.parseObject(http_rest_server.arg("plain"));

    Serial.print("HTTP Method: ");
    Serial.println(http_rest_server.method());
    
    // if (!jsonBody.success()) {
    //     Serial.println("error in parsing json body");
    //     http_rest_server.send(400);
    // }
    // else {   
    //     if (http_rest_server.method() == HTTP_POST) {
    //         if (jsonBody["id"] == 8) {
                
    //             // json_to_resource(jsonBody);
    //             http_rest_server.sendHeader("Status", String("done"));
    //             http_rest_server.send(201);
    //             // pinMode(led_resource.gpio, OUTPUT);
    //         // }
    //         // else if (jsonBody["id"] == 0)
    //         //   http_rest_server.send(404);
    //         // else if (jsonBody["id"] == 2)
    //         //   http_rest_server.send(409);
    //         } else {
    //           http_rest_server.send(409);
    //         }
    //     }
    // }

    if (server_handler_rfid_reset != NULL) {
        server_handler_rfid_reset();
    }

    http_rest_server.send(201);
}

void config_rest_server_routing(void)
{
    http_rest_server.on(REST_SERVER_OPEN_ENDPOINT, HTTP_POST, post_open);
    http_rest_server.on(REST_SERVER_RESET_RFID_ENDPOINT, HTTP_POST, post_rfid_reset);
}

void rest_server_handle_requests(void)
{
    http_rest_server.handleClient();
}

void rest_server_init(void)
{
    config_rest_server_routing();
    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void rest_server_attach_handler_open(void (*f)())
{
    server_handler_open = (f);
}

void rest_server_attach_handler_rfid_reset(void (*f)())
{
    server_handler_rfid_reset = (f);
}