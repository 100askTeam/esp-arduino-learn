/*
  ReadAnalogVoltage （读取引脚电压-可调电阻）

  Reads an analog input on MY_ANALOG_IN_PIN, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).

  https://github.com/100askTeam/arduino-esp32
*/

#define MY_ANALOG_IN_PIN    3

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog MY_ANALOG_IN_PIN:
  int sensorValue = analogRead(MY_ANALOG_IN_PIN);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  Serial.println(voltage);
}