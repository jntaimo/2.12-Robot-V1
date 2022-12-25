#include <Arduino.h>
#include <PS4Controller.h>

//Install https://github.com/aed3/PS4-esp32 for PS4 Remote


void setup() {
  Serial.begin(115200);

  // Replace the "1a:2b:3c:01:01:01" with the MAC address
  // the controller wants to pair to
  // Note: MAC address must be unicast
  PS4.begin("5c:f3:70:a6:b8:38");
  Serial.println("Ready.");
}

void loop() {
  if (PS4.isConnected()) {
    Serial.println("Connected!");
  }

  delay(3000);
}
