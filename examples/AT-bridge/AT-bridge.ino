#include <Arduino.h>
#include <CMMC_NB_IoT.h>
#include "AltSoftSerial.h"
AltSoftSerial nbSerial;


void setup()
{ 
  Serial.begin(57600);
  nbSerial.begin(9600);
  Serial.println("Chiang Mai Maker Club's AT-Bridge engine is started.");
  Serial.setTimeout(2);
  nbSerial.setTimeout(6);
}

String input;
uint8_t hexMode = 0;
void loop()
{ 
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
    nbSerial.write(input.c_str(), input.length());
    nbSerial.write('\r');
  }

  while(nbSerial.available() > 0)  {
      String response = nbSerial.readString(); 
      if (hexMode == 1) { 
        static char buf[3];
        Serial.print("+");
        for(int i =0 ;i < response.length()-1; i++) {
            sprintf(buf, "%02x", response[i]);
            Serial.print(buf);
        }
        Serial.println("-");
      }
      response.trim();
      Serial.println(response);
  }
}
