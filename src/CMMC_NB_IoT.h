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
      String firmware;
      String imei;
      String imsi;
    } DeviceInfo;
    typedef void(*deviceInfoCb_t)(DeviceInfo);
    // constructor
    CMMC_NB_IoT(Stream *s) {
      this->_Serial = s;
      this->_user_debug_cb = [](const char* s) { };
    };
    ~CMMC_NB_IoT() {};

    void init() {
      String buf;
      _writeCommand("AT", 5000);
      _writeCommand("AT+NRB", 10L * 1000);
      _writeCommand("AT+CGSN=1", 10L * 1000, &buf); 

      // Serial.println(buf);

      _writeCommand("AT+CFUN=1", 10L * 1000);
      _writeCommand("AT+NCONFIG=AUTOCONNECT,TRUE", 10L * 1000);
      _writeCommand("AT+CGATT=1", 10L * 1000);
      while (1) {
        String s = "";
        _writeCommand("AT+CGATT?", 10L * 1000, &s, 1);
        if (s.indexOf(F("+CGATT:1")) != -1) {
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

    }

    void onDeviceReboot(voidCb_t cb) {

    }

  private:
    DeviceInfo deviceInfo;
    debugCb_t _user_debug_cb;
    deviceInfoCb_t _user_onDeviceReady_cb;
    Stream *_Serial;

    uint32_t _writeCommand(String at, uint32_t timeoutMs, String *s = NULL, bool silent = false) {
      if (!silent) {
        Serial.print(String("requesting => ") + at); 
      }
      bool reqSuccess = 0;
      at.trim();
      this->_Serial->write(at.c_str(), at.length());
      this->_Serial->write('\r');
      uint32_t startMs = millis();
      timeoutMs = millis() + timeoutMs;
      String buffer;
      while (1) {
        // Serial.println("wait nb rx.. ");
        if (this->_Serial->available()) {
          // String response = this->_Serial->readStringUntil('\n');
          String response = this->_Serial->readString();
          response.trim();
          buffer += response;
          if (response.indexOf(F("OK")) != -1) {
            // Serial.print(String("+++") + response);
            if (!silent) {
              Serial.println(String(" took ") + String(millis() - startMs) + "ms"); 
            }
            reqSuccess = 1;
            *s = buffer;
            break;
          }
          delay(2);
        }
        if (!reqSuccess && ( millis() > timeoutMs) ) {
          reqSuccess = 0;
          Serial.println("wait timeout");
          break;
        }
        delay(2);
      }

      return reqSuccess;
    }


    // on_device_ready;
};

#endif //CMMC_NB_IoT_H
