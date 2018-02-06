#ifndef CMMC_NB_IoT_H
#define CMMC_NB_IoT_H

#include <Arduino.h>
#include "AltSoftSerial.h"

typedef void (*debugCb_t)(const char* msg);
typedef void (*voidCb_t)(void);

#define USER_DEBUG_PRINTF(fmt, args...) { \
    sprintf(this->debug_buffer, fmt, ## args); \
    _user_debug_cb(this->debug_buffer); \
} 


enum UDPConfig { 
  DISABLE_RECV=0, 
  ENABLE_RECV=1,
};
class CMMC_NB_IoT
{
  public:
    typedef struct {
      char firmware[20];
      char imei[20];
      char imsi[20];
    } DeviceInfo;
    typedef void(*deviceInfoCb_t)(DeviceInfo);
    CMMC_NB_IoT(Stream *s);
    ~CMMC_NB_IoT();
    void onDebugMsg(debugCb_t cb); 
    void init(); 
    void onDeviceReady(deviceInfoCb_t cb);
    void onConnecting(voidCb_t cb); 
    void onConnected(voidCb_t cb); 
    void onDeviceReboot(voidCb_t cb);
    bool createUdpSocket(String hostname, uint16_t port, UDPConfig config = DISABLE_RECV) {
      char buffer[40];
      char resBuffer[60];
      // <type>=RAW and <protocol>=6 will be accepted, 
      // but are not supported and should not be used.0
      sprintf(buffer, "AT+NSOCR=DGRAM,17,%d,%d", port, config); 
      this->_writeCommand(buffer, 10L*1000, resBuffer, false);
      String t =String(resBuffer);
      t.replace("OK", "");
      Serial.println(t);
      // (F("AT+NSOCR=DGRAM,17,11223,0"), 10L * 1000, tmp); // create udp
      // nb.createUdpSocket("159.89.205.216", 11223, 1);
    };
    bool _writeCommand(String at, uint32_t timeoutMs, char *s = NULL, bool silent = false);

  private:
    DeviceInfo deviceInfo;
    debugCb_t _user_debug_cb;
    char debug_buffer[60];
    deviceInfoCb_t _user_onDeviceReady_cb;
    voidCb_t _user_onDeviceReboot_cb;
    voidCb_t _user_onConnecting_cb;
    voidCb_t _user_onConnected_cb;
    Stream *_Serial; 
};

#endif //CMMC_NB_IoT_H
