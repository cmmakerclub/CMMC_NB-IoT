#include <Arduino.h>

extern Stream *modem;
extern String input;
extern uint8_t hexMode;

void processUserCommand() {
  if (Serial.available()) {
    Serial.print(">> ");
    input = Serial.readString();
    input.trim();
    input.toUpperCase();
    Serial.println(input);
    if (input == "HEX") {
      Serial.println("HEX MODE ENABLED.");
      hexMode = 1;
    }
    else if (input == "ASCII") {
      Serial.println("HEX MODE DISABLED.");
      hexMode = 0;
    }

    modem->write(input.c_str(), input.length());
    delay(2);
    modem->write('\r');
  }
} 

void listenModemResponse() {
  if (modem->available() > 0)  {
    String response = modem->readString();
    if (hexMode == 1) {
      static char buf[3];
      for (unsigned int i = 0 ; i < response.length() - 1; i++) {
        sprintf(buf, "%02x,", response[i]);
        Serial.print(buf);
      }
    }
    response.trim();
    Serial.println(response);
  }
}
