#include <VirtualWire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#ifndef PSTR
#define PSTR  // Make Arduino Due happy
#endif

#define PIN1 6
#define PIN2 9

Adafruit_NeoMatrix a = Adafruit_NeoMatrix(32, 8, PIN1,NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix b = Adafruit_NeoMatrix(32, 8, PIN2,NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,NEO_GRB + NEO_KHZ800);

const int receivePin = 11;

void setup() {
  // put your setup code here, to run once:
  a.begin();
  a.setTextWrap(false);
  a.setBrightness(40);
  a.setTextColor(a.Color(255, 0, 0));
  b.begin();
  b.setTextWrap(false);
  b.setBrightness(40);
  b.setTextColor(b.Color(255, 0, 0));
  Serial.begin(9600);       // Initialize serial communication for debugging
  vw_set_rx_pin(receivePin);  // Set the receive pin
  vw_setup(2000);             // Transmission speed in bits per second
  vw_rx_start();

}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t buf[VW_MAX_MESSAGE_LEN];                 // Buffer to store received message
  uint8_t buflen = VW_MAX_MESSAGE_LEN;             // Length of the message buffer
  if (vw_get_message(buf, &buflen)) {              // If a message is received
    buf[buflen] = '\0';                            // Add null character to the end of the received buffer
    String receivedMessage = String((char *)buf);  // Convert received buffer to String
    Serial.print("Message received: ");
    Serial.println(receivedMessage);  // Print the received message as a string
    char charArray[receivedMessage.length() + 1];
    receivedMessage.toCharArray(charArray, receivedMessage.length() + 1);

    char *token = strtok(charArray, ",");
    String values[5];  // Assuming a maximum of 10 values, adjust as needed
    int index = 0;

    while (token != NULL && index < 5) {
      values[index] = String(token);
      values[index].trim();
      index++;
      token = strtok(NULL, ",");
    }

    for (int i = 0; i < 5; i++) {
      Serial.println(values[i]);
      if (values[i].equals("ignore")) continue;
      switch (i) {
        case 1:
          {
            if (values[i].length() > 5) break;
            a.fillScreen(0);    // Clear the screen
            a.setCursor(0, 0);  // Set the cursor to the top-left corner
            a.print(values[i]);
            Serial.println(values[i]);
            a.show();    
            break;
          }
        case 2:
          {
            if (values[i].length() > 5) break;
            b.fillScreen(0);    // Clear the screen
            b.setCursor(0, 0);  // Set the cursor to the top-left corner
            b.print(values[i]);
            Serial.print(values[i]);
            b.show();                // Update the display
            break;
          }
      }
    }
  }

}