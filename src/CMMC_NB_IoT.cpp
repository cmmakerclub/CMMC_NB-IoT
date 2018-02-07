#include "CMMC_NB_IoT.h"

// CMMC_NB_IoT::CMMC_NB_IoT(Stream *s) {
//   this->_Serial = s;
//   this->_user_debug_cb = [](const char* s) { };
//   this->_user_onDeviceReboot_cb = [](void) -> void { };
//   this->_user_onConnecting_cb = [](void) -> void { };
//   this->_user_onConnected_cb = [](void) -> void { };
//   this->_user_onDeviceReady_cb = [](DeviceInfo d) -> void { };
// };

CMMC_NB_IoT::~CMMC_NB_IoT() {};

void CMMC_NB_IoT::onDebugMsg(debugCb_t cb) {

  this->_user_debug_cb = cb;
}

void CMMC_NB_IoT::init() { 
  this->_writeCommand(F("AT"), 5L);
  this->_user_onDeviceReboot_cb();
  this->_writeCommand(F("AT+NRB"), 10L);
  this->_writeCommand(F("AT+CFUN=1"), 10L);
  this->_writeCommand(F("AT+CGSN=1"), 10L, this->deviceInfo.imei);  // IMEI
  this->_writeCommand(F("AT+CGMR"), 10L, this->deviceInfo.firmware);  // firmware
  this->_writeCommand(F("AT+CIMI"), 10L, this->deviceInfo.imsi);  // imsi sim


  this->_user_onDeviceReady_cb(this->deviceInfo);
  this->_writeCommand(F("AT+NCONFIG=AUTOCONNECT,TRUE"), 10L * 1000);
  this->_writeCommand(F("AT+CGATT=1"), 10L * 1000);
  char buf[20];
  while (1) {
    String s;
    this->_writeCommand(F("AT+CGATT?"), 10L * 1000, buf, 1);
    String ss = String(buf);
    if (ss.indexOf(F("+CGATT:1")) != -1) {
      USER_DEBUG_PRINTF("%s\n", String("NB-IoT Network Connected.").c_str());
      if (this->_user_onConnected_cb) {
        this->_user_onConnected_cb();
      }
      break;
    }
    else {
      String out = String("[") + millis() + "] Connecting NB-IoT Network...";
      USER_DEBUG_PRINTF("%s\n", out.c_str());
      if (this->_user_onConnecting_cb) {
        this->_user_onConnecting_cb();
      }
    }
  } 
} 

void CMMC_NB_IoT::onDeviceReady(deviceInfoCb_t cb) {
  this->_user_onDeviceReady_cb = cb;
}

void CMMC_NB_IoT::onConnecting(voidCb_t cb) {
  this->_user_onConnecting_cb = cb;
}

void CMMC_NB_IoT::onConnected(voidCb_t cb) {
  this->_user_onConnected_cb = cb;
}

void CMMC_NB_IoT::onDeviceReboot(voidCb_t cb) {
  this->_user_onDeviceReboot_cb = cb;
}

bool CMMC_NB_IoT::_writeCommand(String at, uint32_t timeoutMs, char *s, bool silent) {
  timeoutMs *= 1000L;
  uint32_t startMs = millis();
  timeoutMs = startMs + timeoutMs;
  if (!silent) {
    // USER_DEBUG_PRINTF("%s", at.c_str());
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
  // this->_Serial->write(at.c_str(), at.length());
  for(int i = 0 ; i < at.length(); i++) {
    this->_Serial->print(at[i]); 
    if (i%60 ==0) {
      delay(20);
    }
  }
  this->_Serial->write('\r');
  String nbSerialBuffer="@";
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
        if (s != NULL) {
          strcpy(s, nbSerialBuffer.c_str());
        }
        break;
      }
    } else if ((millis() > timeoutMs) ) {
      reqSuccess = 0;
      USER_DEBUG_PRINTF(nbSerialBuffer.c_str());
      USER_DEBUG_PRINTF(".. wait timeout");
      break;
    } 
  }
  return reqSuccess;
}

