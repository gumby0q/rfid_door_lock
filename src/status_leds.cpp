#include "status_leds.h"
#include <Ticker.h>

Ticker ledsImpulseTicker;

#define _LED_RED_PIN LED_RED_PIN

#define _OPEN_IMPULSE_TIME 1000

#define _OPENING_COUNTER_MAX_VALUE 5
#define _TICKER_DELAY (uint16_t)(_OPEN_IMPULSE_TIME/_OPENING_COUNTER_MAX_VALUE)


volatile int16_t leds_lightning_counter = 0;


void set_pin_low(uint8_t pin);
void set_pin_high(uint8_t pin);
void leds_control_ticker(void);

void status_leds_blink_red(void)
{
    leds_lightning_counter = _OPENING_COUNTER_MAX_VALUE;
}

void status_leds_pin_test(void)
{
/* dissable ticker first */
    delay(2000);
    Serial.println("set_pin_low");
    set_pin_low(_LED_RED_PIN);
    delay(2000);
    Serial.println("set_pin_high");
    set_pin_high(_LED_RED_PIN);
}

void status_leds_blink_red_ticker_test(void)
{
    delay(2000);
    Serial.println("set_pin_low");
    
    status_leds_blink_red();
}

void set_pin_low(uint8_t pin) 
{
    if (digitalRead(pin) != 0) {
        digitalWrite(pin, LOW);
    }
}

/* leave the lock for closing */
void set_pin_high(uint8_t pin)
{
    if (digitalRead(pin) == 0) {
        digitalWrite(pin, HIGH);
    }
}

void status_leds_init(void) 
{
    pinMode(_LED_RED_PIN, OUTPUT);
    digitalWrite(_LED_RED_PIN, HIGH);

    // Serial.print(_TICKER_DELAY);
    // Serial.println(" _TICKER_DELAY");
    ledsImpulseTicker.attach_ms(_TICKER_DELAY, leds_control_ticker);
}

void leds_control_ticker(void) 
{
    if (leds_lightning_counter > 0) {
        leds_lightning_counter--;
        set_pin_low(_LED_RED_PIN);
    } else {
        set_pin_high(_LED_RED_PIN);
    }
}
