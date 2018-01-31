#include <Arduino.h>
#include <CMMC_NB_IoT.h>
#include "AltSoftSerial.h"
AltSoftSerial nbSerial;


void setup()
{ 
  Serial.begin(57600);
  nbSerial.begin(9600);
  Serial.println("CMMC AT-Bridge started.");
  Serial.println("BC-95 is using CR only for line ending.");
}

void loop()
{ 
  if (Serial.available()) {
    Serial.print(">> ");
    while (Serial.available() > 0)  {
      char c = Serial.read();
      Serial.print(c);
      nbSerial.write(c);
      delay(2);
    }
  }

  while (nbSerial.available() > 0)  {
    char c =  char(nbSerial.read());
    Serial.print(c);
    delay(2);
  }
}
