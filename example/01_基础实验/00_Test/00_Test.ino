void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hi, DShanESP32-S3 Arduino!");
  delay(1000);  // wait for a second
}
