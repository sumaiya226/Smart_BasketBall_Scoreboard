#include <VirtualWire.h>

const int transmitPin = 10; // Pin connected to the transmitter module

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging
  vw_set_tx_pin(transmitPin); // Set the transmit pin
  vw_setup(2000); // Transmission speed in bits per second
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readString(); // Read message from Serial Monitor
    const char msg = message.c_str(); // Convert String to const char
    
    vw_send((uint8_t *)msg, strlen(msg)); // Send the message
    vw_wait_tx(); // Wait for transmission to complete

    Serial.print("message sent : ");
    Serial.println(message);
  }
}