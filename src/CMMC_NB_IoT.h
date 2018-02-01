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


class CMMC_NB_IoT
{
  public:
    typedef struct {
      char firmware[20];
      char imei[20];
      char imsi[20];
    } DeviceInfo;
    typedef void(*deviceInfoCb_t)(DeviceInfo);
    // constructor
    CMMC_NB_IoT(Stream *s) {
      this->_Serial = s;
      this->_user_debug_cb = [](const char* s) { };
      this->_user_onDeviceReboot_cb = [](void) -> void { };
      this->_user_onDeviceReady_cb = [](DeviceInfo d) -> void { };
    };
    ~CMMC_NB_IoT() {};

    void onDebugMsg(debugCb_t cb) {
        this->_user_debug_cb = cb;
    }

    void init() {
      _writeCommand(F("AT"), 5L*1000);
      this->_user_onDeviceReboot_cb();
      _writeCommand(F("AT+NRB"), 10L * 1000); 
      _writeCommand(F("AT+CFUN=1"), 10L * 1000);
      _writeCommand(F("AT+CGSN=1"), 10L * 1000, this->deviceInfo.imei);  // IMEI
      _writeCommand(F("AT+CGMR"), 10L * 1000, this->deviceInfo.firmware);  // firmware
      _writeCommand(F("AT+CIMI"), 10L * 1000, this->deviceInfo.imsi);  // imsi sim

      this->_user_onDeviceReady_cb(this->deviceInfo);
      _writeCommand(F("AT+NCONFIG=AUTOCONNECT,TRUE"), 10L * 1000);
      _writeCommand(F("AT+CGATT=1"), 10L * 1000);
      char buf[20];
      while (1) {
        String s;
        _writeCommand(F("AT+CGATT?"), 10L * 1000, buf, 1);
        String ss = String(buf);
        if (ss.indexOf(F("+CGATT:1")) != -1) {
          USER_DEBUG_PRINTF("%s\n", String("NB-IoT Network Connected.").c_str()); 
          break;
        }
        else {
          String out = String("[") + millis() + "] Connecting NB-IoT Network...";
          USER_DEBUG_PRINTF("%s\n", out.c_str()); 
        }
        delay(1000);
      }
    }

    void onDeviceReady(deviceInfoCb_t cb) {
      this->_user_onDeviceReady_cb = cb; 
    }

    void onDeviceReboot(voidCb_t cb) {
      this->_user_onDeviceReboot_cb = cb; 
    }

  private:
    DeviceInfo deviceInfo;
    debugCb_t _user_debug_cb;
    char debug_buffer[60];
    deviceInfoCb_t _user_onDeviceReady_cb;
    voidCb_t _user_onDeviceReboot_cb;
    Stream *_Serial;

    uint32_t _writeCommand(String at, uint32_t timeoutMs, char *s = NULL, bool silent = false) {
      uint32_t startMs = millis();
      timeoutMs = startMs + timeoutMs;
      if (!silent) {
        USER_DEBUG_PRINTF("%s", at.c_str());
        // Serial.print(F("requesting => ")); 
        /*
        Serial.print(F(" start: "));
        Serial.print(startMs);
        Serial.print(F(" timeout: "));
        Serial.print(timeoutMs);
        */
      }
      bool reqSuccess = 0;
      at.trim();
      this->_Serial->write(at.c_str(), at.length());
      this->_Serial->write('\r');
      String nbSerialBuffer;
      while (1) {
        if (this->_Serial->available()) {
          String response = this->_Serial->readStringUntil('\n');
          response.trim();
          nbSerialBuffer += response;
          if (response.indexOf(F("OK")) != -1) {
            if (!silent) {
              String out = String(F(" (")) + String(millis() - startMs) + F("ms)");
              USER_DEBUG_PRINTF("%s\n", out.c_str());
            }
            reqSuccess = 1;
            if (s!= NULL) {
              strcpy(s, nbSerialBuffer.c_str()); 
            }
            break;
          }
        } else if ((millis() > timeoutMs) ) {
          reqSuccess = 0;
          USER_DEBUG_PRINTF("wait timeout at %lu/%lu\n", millis(), timeoutMs);
          break;
        }
        delay(2);
      }
      return reqSuccess;
    }
};

#endif //CMMC_NB_IoT_H
