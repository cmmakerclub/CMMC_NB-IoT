#define DEBUG

#include <Arduino.h>
#include <CMMC_NB_IoT.h> 
#include <AltSoftSerial.h>

CMMC_NB_IoT nb(&Serial2);

void setup()
{
  Serial.begin(57600);
  Serial2.begin(9600);

  Serial.setTimeout(4); 
  Serial2.setTimeout(4); 
  delay(10);
  Serial.println();
  Serial.print(F("Starting Application... at ("));
  Serial.print(millis());
  Serial.println("ms)");

  nb.setDebugStream(&Serial); 

  nb.onDeviceReboot([]() {
    Serial.println(F("[user] Device rebooted."));
  });

  nb.onDeviceReady([]() {
    Serial.println("[user] Device Ready!");
  });

  nb.onDeviceInfo([](CMMC_NB_IoT::DeviceInfo device) {
    Serial.print(F("# Module IMEI-->  "));
    Serial.println(device.imei);
    Serial.print(F("# Firmware ver-->  "));
    Serial.println(device.firmware);
    Serial.print(F("# IMSI SIM-->  "));
    Serial.println(device.imsi);
    Serial.println("Activating NB-IoT shield."); 
  });

  nb.onConnecting([]() {
	  Serial.println("Connecting to NB-IoT...");
    delay(500);
  });

  nb.onConnected([]() {
    Serial.print("[user] NB-IoT Network connected at (");
    Serial.print(millis());
    Serial.println("ms)");
  });

  nb.begin();
  nb.rebootModule(); 
  nb.activate();
}

void loop()
{ 
	nb.loop();
}