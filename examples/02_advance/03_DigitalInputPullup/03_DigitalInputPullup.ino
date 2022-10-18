/*
  Input Pull-up Serial （按键检测的其他用法 -- 上升沿触发）

  This example demonstrates the use of pinMode(INPUT_PULLUP). It reads a digital
  input on pin 2 and prints the results to the Serial Monitor.

  https://github.com/100askTeam/arduino-esp32
*/
// 上升沿触发

#define MY_LED_PIN      1 /*the number of the pushbutton pin*/
#define MY_BUTTON_PIN   2 /*the number of the LED pin*/

void setup() {
  //start serial connection
  Serial.begin(115200);
  
  //configure MY_LED_PIN as an input and enable the internal pull-up resistor
  pinMode(MY_LED_PIN, INPUT_PULLUP);
  pinMode(MY_LED_PIN, OUTPUT);

}

void loop() {
  //read the pushbutton value into a variable
  int sensorVal = digitalRead(2);
  //print out the value of the pushbutton
  Serial.println(sensorVal);

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on MY_LED_PIN when the
  // button's pressed, and off when it's not:
  if (sensorVal == HIGH) {
    digitalWrite(MY_LED_PIN, LOW);
  } else {
    digitalWrite(MY_LED_PIN, HIGH);
  }
}
