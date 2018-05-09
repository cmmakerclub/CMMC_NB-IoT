#include <Arduino.h>
#include <CMMC_NB_IoT.h>
// #include <SoftwareSerial.h>

#define RX 12
#define TX 14


// SoftwareSerial modem(RX, TX, false); 
Stream *modem = 0;

void setup()
{ 
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
  delay(100);
  Serial.begin(57600);
  Serial2.begin(9600);

  modem = &Serial2;
  Serial.println();
  Serial.println("Chiang Mai Maker Club's AT-Bridge engine is started."); 
  Serial.setTimeout(2);
  modem->setTimeout(6);
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
    modem->write(input.c_str(), input.length());

    delay(2);
    modem->write('\r');

  }

  if (modem->available() > 0)  {

      String response = modem->readString(); 

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