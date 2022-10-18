

/*
  LED PWM (Fade) （控制LED灯的亮度）

  This example shows how to fade an LED on MY_LED_PIN using the analogWrite()
  function.

  The analogWrite() function uses PWM, so if you want to change the pin you're
  using, be sure to use another PWM capable pin.

  https://github.com/100askTeam/arduino-esp32
*/

#define MY_LED_PIN    1  /* the PWM pin the LED is attached to */
         
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup() {
  // declare MY_LED_PIN to be an output:
  pinMode(MY_LED_PIN, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // set the brightness of MY_LED_PIN:
  analogWrite(MY_LED_PIN, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}

