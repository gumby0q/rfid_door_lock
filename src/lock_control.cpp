#include "lock_control.h"
#include <Ticker.h>

Ticker impulseTicker;

#define OPEN_IMPULSE_TIME 1000

#define OPENING_COUNTER_MAX_VALUE 5
#define TICKER_DELAY (uint16_t)(OPEN_IMPULSE_TIME/OPENING_COUNTER_MAX_VALUE)


volatile int16_t opening_counter = 0;

void open_lock(void);
void close_lock(void);
void lock_control_ticker(void);

void open(void)
{
    opening_counter = OPENING_COUNTER_MAX_VALUE;
}

void pin_test(void)
{
/* dissable ticker first */
    delay(2000);
    Serial.println("open_lock");
    open_lock();
    delay(2000);
    Serial.println("close_lock");
    close_lock();
}

void open_ticker_test(void)
{
    delay(2000);
    Serial.println("open_lock");
    
    open();
}

void open_lock(void) 
{
    if (digitalRead(PIN_LOCK) != 0) {
        digitalWrite(PIN_LOCK, LOW);
    }
}

/* leave the lock for closing */
void close_lock(void)
{
    if (digitalRead(PIN_LOCK) == 0) {
        digitalWrite(PIN_LOCK, HIGH);
    }
}

void lock_control_init() 
{
    pinMode(PIN_LOCK, OUTPUT);
    digitalWrite(PIN_LOCK, HIGH);

    // Serial.print(TICKER_DELAY);
    // Serial.println(" TICKER_DELAY");
    impulseTicker.attach_ms(TICKER_DELAY, lock_control_ticker);
}

void lock_control_ticker(void) 
{
    if (opening_counter > 0) {
        opening_counter--;
        open_lock();
    } else {
        close_lock();
    }
}
