#define DEBUG_BUFFER_SIZE 80

#include <Arduino.h>
#include <CMMC_Packet.h>


const uint8_t PROJECT = 1;
const uint8_t VERSION = 1;

uint8_t header[2] = {0xfd, 0xfa};
uint8_t tail[2] = {0x0d, 0x0a};

CMMC_Packet packet(PROJECT, VERSION, header, tail);

void setup()
{
  Serial.begin(57600);
  Serial.println("program started.");
  packet.setName("Nat");
  packet.setSensorName("AAA");
  packet.debug([](const char *s) {
    Serial.print(s);
  });

  // const char* hexString = packet.getHexString();
  const CMMC_PACKET_T *ptr = packet.getPacketPtr();
  Serial.println(packet.size());

 
  packet.dump();
}

void loop()
{

}