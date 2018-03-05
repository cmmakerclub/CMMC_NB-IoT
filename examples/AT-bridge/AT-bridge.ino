#include <Arduino.h>
#include <CMMC_NB_IoT.h>
#include <SoftwareSerial.h>

#define RX 12
#define TX 14


SoftwareSerial swSerial(RX, TX, false);

void setup()
{ 
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
  delay(100);
  Serial.begin(57600);
  swSerial.begin(9600);
  Serial.println();
  Serial.println("Chiang Mai Maker Club's AT-Bridge engine is started."); 
  Serial.setTimeout(2);
  swSerial.setTimeout(6);
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
    swSerial.write(input.c_str(), input.length());
    delay(2);
    swSerial.write('\r');
  }

  if (swSerial.available() > 0)  {
      String response = swSerial.readString(); 
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