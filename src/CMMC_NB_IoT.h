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
        _writeCommand("AT"); 
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

      void _writeCommand(String at) {
          at.trim();
          this->_Serial->write(at.c_str(), at.length());
          this->_Serial->write('\r'); 

          uint32_t startMs = millis();
          while(1) {
            Serial.println("wait nb rx.. ");
            if (this->_Serial->available()) {
                String response = this->_Serial->readString(); 
                response.trim();
                Serial.print(String("got response: ") + response); 
                Serial.println(String(" took ") + String(millis() - startMs) + " ms");
                break;
            }
          }
      }


      // on_device_ready;
};

#endif //CMMC_NB_IoT_H
