#include <Arduino.h>
#include <CMMC_NB_IoT.h> 

// #include <AltSoftSerial.h>
CMMC_NB_IoT nb(&Serial2); 

void setup()
{ 
  Serial.begin(57600);
  Serial2.begin(9600); 
  Serial.setTimeout(4);
  // nbSerial.begin(9600); 
  // nbSerial.setTimeout(6);
  Serial2.setTimeout(6);
  delay(10);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println();
  Serial.println(F("Starting application..."));

  nb.onDeviceReboot([]() {
    Serial.println(F("[user] Device being rebooted.")); 
  });

  nb.onDeviceReady([](CMMC_NB_IoT::DeviceInfo device) {
    Serial.println("[user] Device Ready!");
    Serial.print(F("# Module IMEI-->  "));
    Serial.println(device.imei);
    Serial.print(F("# Firmware ver-->  "));
    Serial.println(device.firmware);
    Serial.print(F("# IMSI SIM-->  "));
    Serial.println(device.imsi); 
  }); 

  nb.onConnecting([]() {
    Serial.println("[user] Connecting to NB-IoT Network...");
    delay(1000);
  });

  nb.onConnected([]() {
    Serial.println("[user] NB-IoT Networ connected");
  });

  nb.onDebugMsg([](const char* msg) {
    // Serial.print(msg);
  });

  nb.init(); 
}

void loop()
{ 

}
