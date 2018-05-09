#include <Arduino.h>
#include <CMMC_NB_IoT.h>
#include "user_tasks.hpp"

Stream *modem = &Serial2;
String input;
uint8_t hexMode = 0;


void initSerialPort() {
  Serial.begin(57600);
  Serial2.begin(9600); // modem serialport
  Serial.setTimeout(2);
  modem->setTimeout(6);

  Serial.println();
  Serial.println("Chiang Mai Maker Club's AT-Bridge engine is started.");
  Serial.flush();
  modem->flush();
}

void initGpio() {
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
}

void setup()
{
  initGpio();
  initSerialPort();
}

void loop()
{
  processUserCommand();
  listenModemResponse();
}