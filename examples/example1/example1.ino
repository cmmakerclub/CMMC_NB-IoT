#include <Arduino.h>
#include <CMMC_NB_IoT.h> 

CMMC_NB_IoT nb; 

void setup()
{ 
  Serial.begin(57600);
  nb.init();

  nb.onDeviceRestart([]() {
    Serial.println("Device is restarting..."); 
  });

  nb.onDeviceReady([](CMMC_NB_IoT::DeviceInfo device) {
    Serial.print(F("# Module IMEI-->  "));
    Serial.println(device.imei);
    Serial.print(F("# Firmware ver-->  "));
    Serial.println(device.firmware);
    Serial.print(F("# IMSI SIM-->  "));
    Serial.println(device.imsi); 
  });
}

void loop()
{ 

}
