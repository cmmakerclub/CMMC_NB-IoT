#ifndef CMMC_NB_IoT_H
#define CMMC_NB_IoT_H

#include <Arduino.h> 

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
      CMMC_NB_IoT() { 
        this->_user_debug_cb = [](const char* s) { }; 
      };
      ~CMMC_NB_IoT() {};

      void init() {

      }

      void onDeviceReady(deviceInfoCb_t cb) {

      }

      void onDeviceRestart(voidCb_t cb) {

      }

    private:
      DeviceInfo deviceInfo;
      debugCb_t _user_debug_cb;
      deviceInfoCb_t _user_onDeviceReady_cb;

      // on_device_ready;
};

#endif //CMMC_NB_IoT_H
