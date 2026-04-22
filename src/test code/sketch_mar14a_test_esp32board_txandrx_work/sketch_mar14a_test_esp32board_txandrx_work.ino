void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 44, 43);  // RX=44, TX=43
  Serial.println("UART43/44 loopback test");
}

void loop() {
  Serial1.println("hello");
  delay(200);

  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);
  }
}