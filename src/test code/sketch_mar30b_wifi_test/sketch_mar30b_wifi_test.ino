#include <WiFi.h>

const char* ssid = "iPhone";
const char* password = "LIGHTNING88";

const char* host = "172.20.10.2";   // example: 192.168.1.23
const uint16_t port = 5000;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("Starting Wi-Fi test...");
  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi FAILED");
    return;
  }

  WiFiClient client;
  Serial.println("Connecting to laptop...");

  if (client.connect(host, port)) {
    Serial.println("Connected to laptop server");
    client.println("hello world from AirWear");
    client.stop();
    Serial.println("Message sent");
  } else {
    Serial.println("Failed to connect to laptop server");
  }
}

void loop() {
}