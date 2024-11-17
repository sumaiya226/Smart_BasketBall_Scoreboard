// #include <avr/pgmspace.h>
#include <VirtualWire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#ifndef PSTR
#define PSTR  // Make Arduino Due happy
#endif

// #define PIN1 6
// #define PIN2 10


#define NUM_LEDS_PER_SEGMENT 3
#define NUM_SEGMENTS_PER_DISPLAY 7
#define NUM_DISPLAYS 3
#define LED_PIN1 5  // Input pin for team_a score panel
#define LED_PIN2 8  // Input pin for team_b score panel
#define LED_PIN3 3  // Input pin for timer panel;
#define NUM_LEDS NUM_LEDS_PER_SEGMENT *NUM_SEGMENTS_PER_DISPLAY *NUM_DISPLAYS

Adafruit_NeoPixel team_a(NUM_LEDS, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel team_b(NUM_LEDS, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel timer(84, LED_PIN3, NEO_GRB + NEO_KHZ800);


const int receivePin = 11;  // Pin connected to the receiver module
int minutes = 59, seconds = 59;
int startTimer = false;
//instead of delay
unsigned long previousMillis = 0; // Variable to store the previous time value
const long interval = 1000;

byte scorePattern[10][NUM_SEGMENTS_PER_DISPLAY] = {
  { 1, 1, 1, 1, 1, 1, 0 },  // 0
  { 0, 0, 1, 1, 0, 0, 0 },  // 1
  { 0, 1, 1, 0, 1, 1, 1 },  // 2
  { 0, 1, 1, 1, 1, 0, 1 },  // 3
  { 1, 0, 1, 1, 0, 0, 1 },  // 4
  { 1, 1, 0, 1, 1, 0, 1 },  // 5
  { 1, 1, 0, 1, 1, 1, 1 },  // 6
  { 0, 1, 1, 1, 0, 0, 0 },  // 7
  { 1, 1, 1, 1, 1, 1, 1 },  // 8
  { 1, 1, 1, 1, 1, 0, 1 }   // 9
};
byte timerPattern[10][NUM_SEGMENTS_PER_DISPLAY] = {
  { 0, 1, 1, 1, 1, 1, 1 },  // 0
  { 0, 1, 0, 0, 0, 0, 1 },  // 1
  { 1, 1, 1, 0, 1, 1, 0 },  // 2
  { 1, 1, 1, 0, 0, 1, 1 },  // 3
  { 1, 1, 0, 1, 0, 0, 1 },  // 4
  { 1, 0, 1, 1, 0, 1, 1 },  // 5
  { 1, 0, 1, 1, 1, 1, 1 },  // 6
  { 0, 1, 1, 0, 0, 0, 1 },  // 7
  { 1, 1, 1, 1, 1, 1, 1 },  // 8
  { 1, 1, 1, 1, 0, 1, 1 }   // 9
};

void setup() {
  team_a.begin();
  team_b.begin();
  timer.begin();
  timer.show();
  team_a.show();
  team_b.show();
  Serial.begin(9600);       // Initialize serial communication for debugging
  vw_set_rx_pin(receivePin);  // Set the receive pin
  vw_setup(2000);             // Transmission speed in bits per second
  vw_rx_start();              // Start the receiver
}

void loop() {
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
        case 0:
          {
            if (values[i].equals("start_timer")) startTimer = true;
            if (values[i].equals("stop_timer")) startTimer = false;
            if (values[i].equals("reset_timer")) {
              minutes = 59;
              seconds = 59;
              startTimer = true;
            }
            break;
          }
        case 3:
          {
            Serial.println(values[i]);
            displayScore(values[i].toInt(), "team_a");
            break;
          }
        case 4:
          {
            Serial.println(values[i].toInt());
            displayScore(values[i].toInt(), "team_b");

            break;
          }
      }
    }
  }

  unsigned long currentMillis = millis(); // Get the current time
  if (currentMillis - previousMillis >= interval) { // Check if it's time to update the timer
    previousMillis = currentMillis; // Update the previous time value
    if (startTimer && minutes >= 0) {
      displayTimer(minutes, seconds--);
      if (seconds == 0) {
        seconds = 59;
        minutes--;
      }
      Serial.print(minutes);
      Serial.print(" ");
      Serial.println(seconds);
    }
  }
}

void displayScore(int num, String team) {
  if (num < 0 || num > 999)
    return;  // Handle invalid scores

  int digit;
  int displayOffset = 42;
  for (int i = NUM_DISPLAYS - 1; i >= 0; i--) {
    digit = num % 10;
    num /= 10;
    if (team.equals("team_a")) {
      // Serial.println("iiknoer0");
      displayDigitForA(digit, displayOffset);
    } else {
      displayDigitForB(digit, displayOffset);
    }
    displayOffset -= NUM_SEGMENTS_PER_DISPLAY * NUM_LEDS_PER_SEGMENT;
  }
}

void displayDigitForA(int digit, int offset) {
  for (int i = 0; i < NUM_SEGMENTS_PER_DISPLAY; i++) {
    if (scorePattern[digit][i]) {
      for (int j = 0; j < NUM_LEDS_PER_SEGMENT; j++) {
        team_a.setPixelColor(offset + i * NUM_LEDS_PER_SEGMENT + j, 255, 0, 0);  // Change color as needed (Green)
        // Serial.println("in dfora");
      }
    } else {
      for (int j = 0; j < NUM_LEDS_PER_SEGMENT; j++) {
        team_a.setPixelColor(offset + i * NUM_LEDS_PER_SEGMENT + j, 0, 0, 0);  // Off
      }
    }
  }
  team_a.show();
  Serial.println("team_a");

}

void displayDigitForB(int digit, int offset) {
  for (int i = 0; i < NUM_SEGMENTS_PER_DISPLAY; i++) {
    if (scorePattern[digit][i]) {
      for (int j = 0; j < NUM_LEDS_PER_SEGMENT; j++) {
        team_b.setPixelColor(offset + i * NUM_LEDS_PER_SEGMENT + j, 255, 0, 0);  // Change color as needed (Green)
      }
    } else {
      for (int j = 0; j < NUM_LEDS_PER_SEGMENT; j++) {
        team_b.setPixelColor(offset + i * NUM_LEDS_PER_SEGMENT + j, 0, 0, 0);  // Off
      }
    }
  }
  team_b.show();
  Serial.println("team_b");
}

void displayTimer(int mins, int seconds) {
  int size = 4, digit;
  int offset = 63;
  while (size > 0) {
    if (size > 2) {
      digit = seconds % 10;
      seconds /= 10;
      displayForTimer(digit, offset);
    } else {
      digit = mins % 10;
      mins /= 10;
      displayForTimer(digit, offset);
    }
    offset -= 21;
    size--;
  }
}

void displayForTimer(int digit, int offset) {
  for (int i = 0; i < NUM_SEGMENTS_PER_DISPLAY; i++) {
    if (timerPattern[digit][i]) {
      for (int j = 0; j < NUM_LEDS_PER_SEGMENT; j++) {
        timer.setPixelColor(offset + i * NUM_LEDS_PER_SEGMENT + j, 255, 0, 0);  // Change color as needed (Green)
      }
    } else {
      for (int j = 0; j < NUM_LEDS_PER_SEGMENT; j++) {
        timer.setPixelColor(offset + i * NUM_LEDS_PER_SEGMENT + j, 0, 0, 0);  // Off
      }
    }
  }
  timer.show();
  Serial.print("Timer");
  Serial.print(" ");
  Serial.print(offset);
}