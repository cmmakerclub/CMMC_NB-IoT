#include "CMMC_Packet.h"

void CMMC_Packet::init() { }

void CMMC_Packet::debug(cmmc_debug_cb_t cb) {
  if (cb != NULL) {
    this->_user_debug_cb = cb;
  }
}
CMMC_Packet::CMMC_Packet(uint8_t project, uint8_t version, uint8_t header[2], uint8_t footer[2]) {
  this->_packet.header[0] = header[0];
  this->_packet.header[1] = header[1];
  this->_packet.tail[0] = footer[0];
  this->_packet.tail[1] = footer[1];

  this->_packet.ms = millis();

  // auto blank = [](const char* message) {};
}
CMMC_Packet::~CMMC_Packet() {}

// const char* CMMC_Packet::getHexString() {
//   // toHexString((u8*)&this->_packet, sizeof(this->_packet), this->_hexStringBuffer) ;
//   // return this->_hexStringBuffer;
//   return 0;
// }

void CMMC_Packet::toHexString(const u8 array[], size_t len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}

const CMMC_PACKET_T* CMMC_Packet::getPacketPtr() {
  return &this->_packet;
}

uint32_t CMMC_Packet::checksum(uint8_t* data, size_t len) {
  uint32_t sum = 0;
  while (len--) {
    sum += *(data++);
  }
  return sum;
}

void CMMC_Packet::dump() {
  CMMC_Packet::dump((u8*)&this->_packet, sizeof(this->_packet));
}

void CMMC_Packet::updatePacketSum() {
  unsigned long packetSize = sizeof(this->_packet) - sizeof(this->_packet.sum);
  this->_packet.sum = CMMC_Packet::checksum((uint8_t*) &this->_packet, packetSize);
}

void CMMC_Packet::dump(const u8* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    USER_DEBUG_PRINTF("%02x ", data[i]);
  }
  USER_DEBUG_PRINTF("\n");
}

bool CMMC_Packet::setSensorName(const char name[16]) {
  int len = strlen(name);
  if (len > 15) {
    return false;
  }
  else {
    strcpy(this->_packet.data.sensorName, name);
    this->_packet.data.nameLen = strlen(name);
  }
  updatePacketSum();
  return true;
}

bool CMMC_Packet::setName(const char name[16]) {
  int len = strlen(name);
  if (len > 15) {
    return false;
  }
  else {
    strcpy(this->_packet.myName, name);
    this->_packet.nameLen = strlen(name);
  }
  updatePacketSum();
  return true;
} 