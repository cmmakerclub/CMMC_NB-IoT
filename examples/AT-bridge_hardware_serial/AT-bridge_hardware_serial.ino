#include <Arduino.h>
#include <CMMC_NB_IoT.h>

CMMC_NB_IoT nb;
CMMC_AT_Bridge &Bridge = nb.Bridge; 

void setup()
{
  Serial.begin(57600);
  Serial2.begin(9600); 

  Serial.println("Chiang Mai Maker Club's AT-Bridge engine is started.");
  Bridge.init(&Serial, &Serial2); 
}

void loop()
{
  Bridge.loop();
}