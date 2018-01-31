#include <Arduino.h>
#include <AltSoftSerial.h>
#include <CMMC_NB_IoT.h> 

AltSoftSerial nbSerial;
CMMC_NB_IoT nb(&nbSerial); 

void setup()
{ 
  Serial.begin(57600);
  nbSerial.begin(9600); 
  Serial.setTimeout(4);
  nbSerial.setTimeout(6);
  Serial.println("Starting application...");

  nb.onDeviceReboot([]() {
    Serial.println("Device being rebooted."); 
  });

  nb.onDeviceReady([](CMMC_NB_IoT::DeviceInfo device) {
    Serial.print(F("# Module IMEI-->  "));
    Serial.println(device.imei);
    Serial.print(F("# Firmware ver-->  "));
    Serial.println(device.firmware);
    Serial.print(F("# IMSI SIM-->  "));
    Serial.println(device.imsi); 
  });

  nb.init(); 
}

void loop()
{ 

}
