/*
  Blink （点亮LED灯）

  Turns an LED on for one second, then off for one second, repeatedly.

  https://github.com/100askTeam/arduino-esp32
*/

#define MY_LED_PIN 1

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin MY_LED_PIN as an output.
  pinMode(MY_LED_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(MY_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(MY_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
