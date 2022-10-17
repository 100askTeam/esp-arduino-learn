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

// ensure this library description is only included once
#ifndef FC_JOYPAD_100ASK_H
#define FC_JOYPAD_100ASK_H


typedef enum {
  FC_JOYPAD_BTN_A = 0,
  FC_JOYPAD_BTN_B,
  FC_JOYPAD_BTN_SELECT,
  FC_JOYPAD_BTN_START,
  FC_JOYPAD_BTN_UP,
  FC_JOYPAD_BTN_DOWN,
  FC_JOYPAD_BTN_LEFT,
  FC_JOYPAD_BTN_RIGHT,
} fc_joypad_btn_t;

typedef enum {
    FC_JOYPAD_100ASK_RELEASE = 0,
    FC_JOYPAD_100ASK_PRESSED,
} fc_joypad_state_t;

// library interface description
class FcJoypad_100ask {
  public:
    // constructors:
    FcJoypad_100ask(int8_t clock_pin, int8_t latch_pin, int8_t data_pin);

    // initialization
    void begin(void);
    void read(void);

    bool state(void);
    bool isPressed(fc_joypad_btn_t btn);
    bool isRelease(fc_joypad_btn_t btn);

  private:
    int8_t clock_pin;
    int8_t latch_pin;
    int8_t data_pin;
    
    int8_t player;
    bool state_value;
    
};

#endif /*FC_JOYPAD_100ASK_H*/

