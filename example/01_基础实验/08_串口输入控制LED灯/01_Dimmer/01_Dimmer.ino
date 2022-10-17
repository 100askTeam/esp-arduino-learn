/*
  Dimmer

  Demonstrates sending data from the computer to the DShanMCU board, in this case
  to control the brightness of an LED. The data is sent in individual bytes,
  each of which ranges from 0 to 255. DShanMCU reads these bytes and uses them to
  set the brightness of the LED.

  https://github.com/100askTeam/arduino-esp32
*/

#define MY_LED_PIN  1     /*the pin that the LED is attached to*/

void setup() {
  // initialize the serial communication:
  Serial.begin(115200);
  // initialize the MY_LED_PIN as an output:
  pinMode(MY_LED_PIN, OUTPUT);
}

void loop() {
  byte brightness;

  // check if data has been sent from the computer:
  if (Serial.available()) {
    // read the most recent byte (which will be from 0 to 255):
    brightness = Serial.read();
    // set the brightness of the LED:
    analogWrite(MY_LED_PIN, brightness);
  }
}