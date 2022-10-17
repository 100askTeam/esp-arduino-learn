/*
 ***********************************************
    Document: http://esp32.100ask.net
    Forum:    https://forums.100ask.net
    Github:   https://github.com/espressif/arduino-esp32
 ***********************************************
    FC_JOYPAD_100ASK_CLOCK_PIN	----- ID
    FC_JOYPAD_100ASK_LATCH_PIN	----- D+
    FC_JOYPAD_100ASK_DATA_PIN	----- D-
 ***********************************************
    A:      0000 0001 1     (1 << 0)
    B:      0000 0010 2     (1 << 1)
    start: 	0000 0100 8     (1 << 2)
    select:	0000 1000 4     (1 << 3)
    Up:     0001 0000 16    (1 << 4)
    Down:   0010 0000 32    (1 << 5)
    Left:   0100 0000 64    (1 << 6)
    Right:  1000 0000 128   (1 << 7)
 ***********************************************
*/
#include <FcJoypad_100ask.h>

#define FC_JOYPAD_100ASK_CLOCK_PIN	40		/*ID*/
#define FC_JOYPAD_100ASK_LATCH_PIN	41		/*D+*/
#define FC_JOYPAD_100ASK_DATA_PIN	  42		/*D-*/

// initialize the FcJoypad library
FcJoypad_100ask myFcJoypad(FC_JOYPAD_100ASK_CLOCK_PIN, FC_JOYPAD_100ASK_LATCH_PIN, FC_JOYPAD_100ASK_DATA_PIN);

void show_joypad_state_info(fc_joypad_btn_t btn, bool btn_state) {
  switch (btn)
  {
    case FC_JOYPAD_BTN_A:
      Serial.print("Key FC_JOYPAD_BTN_A is ");
      break;
    case FC_JOYPAD_BTN_B:
      Serial.print("Key FC_JOYPAD_BTN_B is ");
      break;
    case FC_JOYPAD_BTN_START:
      Serial.print("Key FC_JOYPAD_BTN_START is ");
      break;
    case FC_JOYPAD_BTN_SELECT:
      Serial.print("Key FC_JOYPAD_BTN_SELECT is ");
      break;
    case FC_JOYPAD_BTN_UP:
      Serial.print("Key FC_JOYPAD_BTN_UP is ");
      break;
    case FC_JOYPAD_BTN_DOWN:
      Serial.print("Key FC_JOYPAD_BTN_DOWN is ");
      break;
    case FC_JOYPAD_BTN_LEFT:
      Serial.print("Key FC_JOYPAD_BTN_LEFT is ");
      break;
    case FC_JOYPAD_BTN_RIGHT:
      Serial.print("Key FC_JOYPAD_BTN_RIGHT is ");
      break;
    default:
      Serial.print("Data error!");
      break;
  }

  if (btn_state)
    Serial.println("pressed!");
  else
    Serial.println("release!");

}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  myFcJoypad.begin();
}

// the loop function runs over and over again forever
void loop() {
  static uint8_t count = 0;

  myFcJoypad.read();
  for (count = 0; count < 8; count++)
  {
    if (myFcJoypad.isPressed((fc_joypad_btn_t)count))
      show_joypad_state_info((fc_joypad_btn_t)count, 1);
    else if (myFcJoypad.isRelease((fc_joypad_btn_t)count))
      show_joypad_state_info((fc_joypad_btn_t)count, 0);
  }

}
