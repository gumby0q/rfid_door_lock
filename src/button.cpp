#include "button.h"

#include <Ticker.h>

#define BUTTON_COUNTER_MAX_VALUE 5
#define BUTTON_TICKER_DELAY (uint16_t)(DEBOUNCE_TIME_MS/BUTTON_COUNTER_MAX_VALUE)

volatile int16_t debounce_counter = 0;
volatile uint8_t button_status = 1;
volatile uint8_t shadow_button_status = 1;
Ticker buttonTicker;

typedef void (*handler)(void);

handler button_handler;

void button_ticker(void);

void button_trigger(void);


void button_trigger(uint8_t button_status)
{
    Serial.print("button_status ");
    Serial.print(button_status);
    Serial.println(" ");

    if (button_status == 0) {
        if (button_handler != NULL) {
            button_handler();
        }
    }
}

void button_ticker(void)
{
    button_status = digitalRead(PIN_BUTTON);
    if (button_status != shadow_button_status) {
        debounce_counter++;

        if (debounce_counter >= BUTTON_COUNTER_MAX_VALUE) {
            debounce_counter = BUTTON_COUNTER_MAX_VALUE;

            shadow_button_status = button_status;
            button_trigger(button_status);
        }
    } else {
        debounce_counter = 0;
    }
}

void button_init(void)
{
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    digitalWrite(PIN_BUTTON, LOW);

    buttonTicker.attach_ms(BUTTON_TICKER_DELAY, button_ticker);
}

void button_attach_handler(void (*f)())
{
    button_handler = (f);
}

