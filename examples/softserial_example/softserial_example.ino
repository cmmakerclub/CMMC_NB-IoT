#define DEBUG_BUFFER_SIZE 1000

#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <CMMC_NB_IoT.h>
#include <CMMC_Packet.h> 
#include <CMMC_LED.h>
#include <SoftwareSerial.h>

#define RX 12
#define TX 14
static SoftwareSerial modemSerial(RX, TX, false, 128);

const uint8_t PROJECT = 1;
const uint8_t VERSION = 1;

uint8_t header[2] = {0xfd, 0xfa};
uint8_t tail[2] = {0x0d, 0x0a};

static CMMC_Packet packet(PROJECT, VERSION, header, tail); 
static CMMC_NB_IoT nb(&modemSerial);
static CMMC_LED led(2, LOW);
static CMMC_LED shield(15, HIGH);

extern "C" {
#include "user_interface.h"
}

void setup(){
  // system_update_cpu_freq(80);
  led.init();
  shield.init(); 

  Serial.begin(57600);
  modemSerial.begin(9600);
  Serial.println(ESP.getCpuFreqMHz());
  Serial.println(modemSerial.baudRate());

  Serial.setTimeout(4); 
  modemSerial.setTimeout(8);

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin(); 

  delay(50);
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
    led.high();
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
    led.toggle();
    delay(1000);
  });

  nb.onConnected([=]() {
    Serial.println("[user] NB-IoT Network connected");
    uint32_t ct = 1;
    String _tmp = "";
    int sockId = nb.createUdpSocket("159.89.205.216", 11221, UDPConfig::DISABLE_RECV);
    delay(100);
    while(1) {
      packet.setField(1, 0x11);
      packet.setField(2, 0x22);
      packet.setField(3, 0x33);
      packet.setField(4, 0x44);
      packet.setField(5, 0x55);
      packet.setField(6, 0x66);
      packet.setSensorBattery(analogRead(A0));
      packet.setSensorName("LATTE-0x10");
      CMMC_PACKET_T *p = packet.getRawPacket();
      uint32_t nextTick = millis() + 2000;

      Serial.printf("sending result: %d\r", nb.sendMessage((uint8_t*)p, packet.size())); 

      int runCnt = 0;
      while(runCnt++ < 65) { 
        led.toggle();
        delay(100);
        if (millis() > nextTick) {
          Serial.println("Sending...");
          while(!nb.sendMessage((uint8_t*)p, packet.size())) {
            delay(100);
          }
          nextTick += 2000;
        }
      }
      Serial.println("BYE..");
      delay(1000);
      led.high(); 
      shield.low();
      ESP.deepSleep(20e6, WAKE_RF_DISABLED);
    }
  });

  nb.onDebugMsg([](const char* msg) {
    Serial.print(msg);
  });

  nb.begin();
}

void loop()
{

}