# CMMC_NB-IoT  [![Build Status](https://travis-ci.org/cmmakerclub/CMMC_NB-IoT.svg?branch=master)](https://travis-ci.org/cmmakerclub/CMMC_NB-IoT)

    #include <CMMC_NB_IoT.h>
    CMMC_NB_IoT nb(&Serial2);


    void setup() {
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
        Serial.println("[user] NB-IoT Network connected");
      });
      
      nb.onResponse([](CMMC_NB_IoT::UDPReceive resp){
        Serial.println("socket : ");
        Serial.print(resp.socket);
        Serial.println("ip_address : ");
        Serial.print(resp.ip_address);
        Serial.println("port : ");
        Serial.print(resp.port);
        Serial.println("length : ");
        Serial.print(resp.length);
        Serial.println("data : ");
        Serial.print(resp.data);
        Serial.println("remaining_length : ");
        Serial.print(resp.remaining_length);
      });


      nb.begin();
    }

    void loop()
    {
      nb.loop();
    }

## Dependencies
 - https://github.com/PaulStoffregen/AltSoftSerial
