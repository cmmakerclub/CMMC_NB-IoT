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
        userSerial->print(">> ");
        input = userSerial->readString();
        input.trim();
        input.toUpperCase();
        userSerial->println(input);
        if (input == "HEX") {
          userSerial->println("HEX MODE ENABLED.");
          hexMode = 1;
        }
        else if (input == "ASCII") {
          userSerial->println("HEX MODE DISABLED.");
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

};

#endif