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

static CMMC_Packet packet(PROJECT, VERSION, header, tail); 
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

  nb.onConnected([=]() {
    Serial.println("[user] NB-IoT Network connected");
    uint32_t ct = 1;
    String _tmp = "";
    int sockId = nb.createUdpSocket("159.89.205.216", 11221, UDPConfig::DISABLE_RECV);
    while(1) {
      uint32_t t = millis();
      Serial.println(ct);
      packet.setField(1, 0x11);
      packet.setField(2, 0x22);
      packet.setField(3, 0x33);
      packet.setField(4, 0x44);
      packet.setField(5, 0x55);
      packet.setField(6, 0x66);
      packet.setSensorBattery(ct);
      packet.setSensorName("HELLO");
      CMMC_PACKET_T *p = packet.getRawPacket();
      p->data.field5 = 0x55;
      nb.sendMessage((uint8_t*)p, packet.size());
      // if (nb.sendMessage(_tmp, 0)) {
      //   Serial.println(String("Data Sent .. ") + (millis() - t) + "ms"); 
      // }
      // else {
      //   Serial.println(String("Send failed.. ") + (millis() - t) + "ms"); 
      // }
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