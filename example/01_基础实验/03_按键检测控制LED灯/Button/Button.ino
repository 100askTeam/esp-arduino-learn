/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  https://github.com/100askTeam/arduino-esp32
*/

#define MY_LED_PIN      1
#define MY_BUTTON_PIN   2

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(MY_LED_PIN, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(MY_BUTTON_PIN, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(MY_BUTTON_PIN);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(MY_LED_PIN, HIGH);
  } else {
    // turn LED off:
    digitalWrite(MY_LED_PIN, LOW);
  }
}
