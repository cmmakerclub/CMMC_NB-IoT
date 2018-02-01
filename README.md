# CMMC_NB-IoT  [![Build Status](https://travis-ci.org/cmmakerclub/CMMC_NB-IoT.svg?branch=master)](https://travis-ci.org/cmmakerclub/CMMC_NB-IoT)

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
    
[Example Code](https://github.com/cmmakerclub/CMMC_NB-IoT/blob/master/examples/example1/example1.ino)



## Dependencies (provided)
 - https://github.com/PaulStoffregen/AltSoftSerial
