#ifndef CMMC_NB_IoT_H
#define CMMC_NB_IoT_H

#include <Arduino.h>
#include "AltSoftSerial.h"

typedef void (*debugCb_t)(const char* msg);
typedef void (*voidCb_t)(void);

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

    void init() {
      char buf[100];
      _writeCommand(F("AT"), 5L*1000);
      _writeCommand(("AT+NRB"), 10L * 1000);
      _writeCommand("AT+CIMI", 10L * 1000);  // imsi sim
      strcpy(this->deviceInfo.imsi, buf);
      _writeCommand(F("AT+CGSN=1"), 10L * 1000, buf);  // IMEI
      strcpy(this->deviceInfo.imei, buf);
      Serial.println(buf);
      _writeCommand(F("AT+CGMR"), 10L * 1000, buf);  // firmware
      strcpy(this->deviceInfo.firmware, buf);
      _writeCommand(F("AT+CFUN=1"), 10L * 1000);
      // _writeCommand(F("AT+NCONFIG=AUTOCONNECT,TRUE"), 10L * 1000);
      this->_user_onDeviceReady_cb(this->deviceInfo);
      _writeCommand(F("AT+CGATT=1"), 10L * 1000);
      while (1) {
        String s;
        _writeCommand("AT+CGATT?", 10L * 1000, buf, 1);
        String ss = String(buf);
        if (ss.indexOf(F("+CGATT:1")) != -1) {
          Serial.println("NB-IoT Network Connected.");
          break;
        }
        else {
          Serial.println(String("[") + millis() + "] Connecting NB-IoT Network...");
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
    deviceInfoCb_t _user_onDeviceReady_cb;
    voidCb_t _user_onDeviceReboot_cb;
    Stream *_Serial;

    uint32_t _writeCommand(String at, uint32_t timeoutMs, char *s = NULL, bool silent = false) {
      uint32_t startMs = millis();
      timeoutMs = startMs + timeoutMs;
      if (!silent) {
        Serial.print(F("requesting => ")); 
        Serial.print(at);
        // Serial.print(F(" start: "));
        // Serial.print(startMs);
        // Serial.print(F(" timeout: "));
        // Serial.print(timeoutMs);
      }
      bool reqSuccess = 0;
      at.trim();
      this->_Serial->write(at.c_str(), at.length());
      this->_Serial->write('\r');
      String nbSerialBuffer;
      while (1) {
        // if (millis() % 1000 == 0) 
          // Serial.println("wait nb rx.. ");
        if (this->_Serial->available()) {
          String response = this->_Serial->readStringUntil('\n');
          response.trim();
          nbSerialBuffer += response;
          if (response.indexOf(F("OK")) != -1) {
            // Serial.print(String("+++") + nbSerialBuffer);
            if (!silent) {
              Serial.println(String(F(" (")) + String(millis() - startMs) + F("ms)")); 
            }
            reqSuccess = 1;
            // *s = nbSerialBuffer;
            if (s!= NULL) {
              strcpy(s, nbSerialBuffer.c_str()); 
            }
            break;
          }
        } else if ((millis() > timeoutMs) ) {
          reqSuccess = 0;
          Serial.println("wait timeout");
          Serial.print(millis());
          Serial.print(" ");
          Serial.println(timeoutMs);
          break;
        }
        delay(2);
      }

      return reqSuccess;
    }


    // on_device_ready;
};

#endif //CMMC_NB_IoT_H
