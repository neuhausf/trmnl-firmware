#include <Arduino.h>
#include <pins.h>
#include <config.h>

void pins_init(void)
{
    #ifndef FIREBEETLE
    pinMode(PIN_INTERRUPT, INPUT);
    pinMode(PIN_RESET, INPUT_PULLUP);
    #endif
}

void pins_set_clear_interrupt(void (*f)(void))
{
    #ifndef FIREBEETLE
    attachInterrupt(PIN_RESET, f, RISING);
    #endif
}