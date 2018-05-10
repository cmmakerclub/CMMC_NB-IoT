#ifndef CMMC_AT_BRIDGE_HPP
#define CMMC_AT_BRIDGE_HPP
#include <Arduino.h>

class CMMC_AT_Bridge
{
  public:
    void init(Stream *input, Stream* modem) {
      this->modem = modem;
      this->userSerial = input;

      this->userSerial->setTimeout(2);
      this->modem->setTimeout(2);
    }

    void processUserCommand() {
      if (userSerial->available()) {
        input = userSerial->readString();
        input.trim();
        input.toUpperCase();
        if (echoMode) {
          userSerial->print(">> ");
          userSerial->println(input);
        }
        if (input == "HEX") {
          hexMode = !hexMode;
          if (hexMode) {
            userSerial->println("HEX MODE ENABLED.");
          }
          else {
            userSerial->println("HEX MODE DISABLED.");
          }
        }
        else if (input == "ECHO") {
          echoMode = !echoMode;
          if (echoMode) {
            userSerial->println("ECHO MODE ENABLED.");
          }
          else {
            userSerial->println("ECHO MODE DISABLED.");
          }
        }
        else {
          modem->write(input.c_str(), input.length());
          delay(2);
          modem->write('\r');
        }
      }
    }

    void listenModemResponse() {
      if (modem->available() > 0)  {
        String response = modem->readString();
        if (hexMode == 1) {
          static char buf[3];
          for (unsigned int i = 0 ; i < response.length() - 1; i++) {
            sprintf(buf, "%02x,", response[i]);
            userSerial->print(buf);
          }
        }
        response.trim();
        userSerial->println(response);
      }
    }


    void loop() {
      processUserCommand();
      listenModemResponse();
    }
  private:
    Stream *modem = NULL;
    Stream *userSerial = NULL;
    String input;
    uint8_t hexMode = 0;
    bool echoMode = false;
};

#endif