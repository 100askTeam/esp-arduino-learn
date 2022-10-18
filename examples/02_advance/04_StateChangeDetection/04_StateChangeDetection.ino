/*
  State change detection (edge detection) （按键检测的其他用法 -- 边缘检测）

  Often, you don't need to know the state of a digital input all the time, but
  you just need to know when the input changes from one state to another.
  For example, you want to know when a button goes from OFF to ON. This is called
  state change detection, or edge detection.

  This example shows how to detect when a button or button changes from off to on
  and on to off.

  https://github.com/100askTeam/arduino-esp32
*/

// 边缘检测

#define MY_LED_PIN      1 /*the number of the pushbutton pin*/
#define MY_BUTTON_PIN   2 /*the number of the LED pin*/

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int lastButtonState = 0;     // previous state of the button
int buttonState = 0;         // current state of the button

void setup() {
  // initialize serial communication:
  Serial.begin(115200);

  // initialize the button pin as a input:
  pinMode(MY_BUTTON_PIN, INPUT);
  // initialize the LED as an output:
  pinMode(MY_LED_PIN, OUTPUT);
}


void loop() {
  // read the pushbutton input pin:
  buttonState = digitalRead(MY_BUTTON_PIN);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;


  // turns on the LED every four button pushes by checking the modulo of the
  // button push counter. the modulo function gives you the remainder of the
  // division of two numbers:
  if (buttonPushCounter % 4 == 0) {
    digitalWrite(MY_LED_PIN, HIGH);
  } else {
    digitalWrite(MY_LED_PIN, LOW);
  }

}
