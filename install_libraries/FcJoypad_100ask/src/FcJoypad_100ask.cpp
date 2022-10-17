/*
 ***************************************
 *  CLOCK_PIN	----- ID
 *  LATCH_PIN	----- D+
 *  DATA_PIN	----- D-
 ***************************************
 *  A:      0000 0001 1     (1 << 0)
 *  B:      0000 0010 2     (1 << 1)
 *  start: 	0000 0100 8     (1 << 2)
 *  select:	0000 1000 4     (1 << 3)
 *  Up:     0001 0000 16    (1 << 4)
 *  Down:   0010 0000 32    (1 << 5)
 *  Left:   0100 0000 64    (1 << 6)
 *  Right:  1000 0000 128   (1 << 7)
 ***************************************
*/ 

#include "Arduino.h"
#include "FcJoypad_100ask.h"


FcJoypad_100ask::FcJoypad_100ask(int8_t clock_pin, int8_t latch_pin, int8_t data_pin)
{
    this->clock_pin = clock_pin;
    this->latch_pin = latch_pin;
    this->data_pin = data_pin;
    this->player = 0;
    this->state_value = true;
}


void FcJoypad_100ask::begin(void)
{
    pinMode(this->clock_pin, OUTPUT);
    pinMode(this->latch_pin, OUTPUT);
    pinMode(this->data_pin, INPUT);
}


void FcJoypad_100ask::read(void)
{
    uint8_t count = 0;

    digitalWrite(this->latch_pin, HIGH);
    delay(1);
    digitalWrite(this->latch_pin, LOW);
    delay(1);

    this->player = 0;
    this->state_value = false;
    for (count = 0; count < 8; count++)
    {
        if (digitalRead(this->data_pin) == LOW){
            this->player |= (1 << count);
            this->state_value = true;
        }
        
        digitalWrite(this->clock_pin, HIGH);
        delay(1);
        digitalWrite(this->clock_pin, LOW);
        delay(1);
    }
}

bool FcJoypad_100ask::state(void) {
  if(this->state_value)
        return FC_JOYPAD_100ASK_PRESSED;
    else
        return FC_JOYPAD_100ASK_RELEASE;
}

bool FcJoypad_100ask::isPressed(fc_joypad_btn_t btn) {
  if ((((uint8_t)1 << btn) & this->player) >= 1)
    return true;
  else
    return false;
}


bool FcJoypad_100ask::isRelease(fc_joypad_btn_t btn) {
  if ((((uint8_t)1 << btn) & this->player) >= 1)
    return true;
  else
    return false;
}

