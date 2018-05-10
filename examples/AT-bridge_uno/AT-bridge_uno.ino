#include <Arduino.h>
#include <CMMC_NB_IoT.h>
#include <AltSoftSerial.h>

CMMC_NB_IoT nb;
CMMC_AT_Bridge &Bridge = nb.Bridge; 
AltSoftSerial altsoftSerial;

void setup()
{
  Serial.begin(57600);
  altsoftSerial.begin(9600); 

  Serial.println("Chiang Mai Maker Club's AT-Bridge engine is started.");
  Bridge.init(&Serial, &altsoftSerial); 
}

void loop()
{
  Bridge.loop();
}