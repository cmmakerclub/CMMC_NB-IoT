#define DEBUG_BUFFER_SIZE 1000

#include <Arduino.h>
#include <CMMC_NB_IoT.h>
#include <CMMC_Packet.h> 
#include <SoftwareSerial.h>

#define RX 14
#define TX 12
static SoftwareSerial modemSerial(RX, TX, false, 256);

const uint8_t PROJECT = 1;
const uint8_t VERSION = 1;

uint8_t header[2] = {0xfd, 0xfa};
uint8_t tail[2] = {0x0d, 0x0a};

CMMC_Packet packet(PROJECT, VERSION, header, tail); 
CMMC_NB_IoT nb(&modemSerial);

void setup()
{
  Serial.begin(57600);
  modemSerial.begin(9600);

  Serial.setTimeout(4); 
  modemSerial.setTimeout(4);
  
  delay(10);
  Serial.println();
  Serial.println(F("Starting application..."));

  while(!modemSerial) {
    Serial.println("wait modem serial..");
    delay(10);
  } 

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
    // Serial.println("[user] Connecting to NB-IoT Network...");
    delay(1000);
  });

  nb.onConnected([]() {
    Serial.println("[user] NB-IoT Network connected");
    int ct = 1;
    String _tmp = "";
    int sockId = nb.createUdpSocket("159.89.205.216", 11221, UDPConfig::DISABLE_RECV);
    while(1) {
      _tmp += String(ct) + "-";
      Serial.println(String("payload size = ") + (_tmp.length()) + String("byte"));
      uint32_t t = millis();
      if (nb.sendMessage(_tmp, 0)) {
        Serial.println(String("Data Sent .. ") + (millis() - t) + "ms"); 
      }
      else {
        Serial.println(String("Send failed.. ") + (millis() - t) + "ms"); 
      }
      delay(2L*1000);
      ct++;
    }
  });

  nb.onDebugMsg([](const char* msg) {
    Serial.print(msg);
  });

  nb.begin(&modemSerial);
}

void loop()
{

}