/*
  MultipleBlink （循环点亮LED灯-流水灯）

  Multiple blink.

  https://github.com/100askTeam/arduino-esp32
*/

#define MY_LED1_PIN     1
#define MY_LED2_PIN     2
#define MY_LED3_PIN     3
#define MY_LED_NUMBER   3

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin MY_LED1_PIN ~ MY_LED3_PIN as an output.
  pinMode(MY_LED1_PIN, OUTPUT);
  pinMode(MY_LED2_PIN, OUTPUT);
  pinMode(MY_LED3_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(MY_LED1_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(MY_LED1_PIN, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(MY_LED2_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(MY_LED2_PIN, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(MY_LED3_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(MY_LED3_PIN, LOW);    // turn the LED off by making the voltage LOW
}
