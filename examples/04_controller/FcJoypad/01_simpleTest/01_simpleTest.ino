/*
 ***********************************************
    Document: http://esp32.100ask.net
    Forum:    https://forums.100ask.net
    Github:   https://github.com/espressif/arduino-esp32
 ***********************************************
    CLOCK_PIN	----- ID
    LATCH_PIN	----- D+
    DATA_PIN	----- D-
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

#define FC_JOYPAD_100ASK_CLOCK_PIN 40 /*ID*/
#define FC_JOYPAD_100ASK_LATCH_PIN 41 /*D+*/
#define FC_JOYPAD_100ASK_DATA_PIN 42  /*D-*/

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


static volatile uint8_t player = 0;
static uint8_t count;


void show_joypad_state_info(fc_joypad_btn_t btn) {
  switch (btn) {
    case FC_JOYPAD_BTN_A:
      Serial.println("Key FC_JOYPAD_BTN_A is pressed!");
      break;
    case FC_JOYPAD_BTN_B:
      Serial.println("Key FC_JOYPAD_BTN_B is pressed!");
      break;
    case FC_JOYPAD_BTN_START:
      Serial.println("Key FC_JOYPAD_BTN_START is pressed!");
      break;
    case FC_JOYPAD_BTN_SELECT:
      Serial.println("Key FC_JOYPAD_BTN_SELECT is pressed!");
      break;
    case FC_JOYPAD_BTN_UP:
      Serial.println("Key FC_JOYPAD_BTN_UP is pressed!");
      break;
    case FC_JOYPAD_BTN_DOWN:
      Serial.println("Key FC_JOYPAD_BTN_DOWN is pressed!");
      break;
    case FC_JOYPAD_BTN_LEFT:
      Serial.println("Key FC_JOYPAD_BTN_LEFT is pressed!");
      break;
    case FC_JOYPAD_BTN_RIGHT:
      Serial.println("Key FC_JOYPAD_BTN_RIGHT is pressed!");
      break;
    default:
      Serial.println("Data error!");
      break;
  }
}

bool btn_is_press(uint8_t joyad_data, fc_joypad_btn_t btn) {
  if ((((uint8_t)1 << btn) & joyad_data) >= 1)
    return true;
  else
    return false;
}


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  // initialize digital pin.
  pinMode(FC_JOYPAD_100ASK_CLOCK_PIN, OUTPUT);
  pinMode(FC_JOYPAD_100ASK_LATCH_PIN, OUTPUT);
  pinMode(FC_JOYPAD_100ASK_DATA_PIN, INPUT);

  player = 0;
  count = 0;
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(FC_JOYPAD_100ASK_LATCH_PIN, HIGH);
  delay(1);
  digitalWrite(FC_JOYPAD_100ASK_LATCH_PIN, LOW);
  delay(1);

  player = 0;
  for (count = 0; count < 8; count++) {
    if (digitalRead(FC_JOYPAD_100ASK_DATA_PIN) == LOW)
      player |= (1 << count);
    digitalWrite(FC_JOYPAD_100ASK_CLOCK_PIN, HIGH);
    delay(1);
    digitalWrite(FC_JOYPAD_100ASK_CLOCK_PIN, LOW);
    delay(1);
  }

  //Serial.println(player);

  for (count = 0; count < 8; count++) {
    if (btn_is_press(player, (fc_joypad_btn_t)count))
      show_joypad_state_info((fc_joypad_btn_t)count);
  }
}