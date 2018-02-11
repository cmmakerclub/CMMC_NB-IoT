#include "CMMC_NB_IoT.h"

// CMMC_NB_IoT::CMMC_NB_IoT(Stream *s) {
//   this->_modemSerial = s;
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

bool CMMC_NB_IoT::callCommand(String at, uint8_t timeout, int retries, char *outStr) {
  int r = 0;
  int ok = false;
  while (r < retries) {
    if (this->_writeCommand(at.c_str(), timeout, outStr) == 1) {
      ok = true;
      break;
    }
    r++;
    delay(1000);
    USER_DEBUG_PRINTF("%s [%d]\n", r+1, at.c_str());
  }
  delay(50);
  return ok;
}

void CMMC_NB_IoT::begin(Stream *s) {
  if (s != 0) {
    this->_modemSerial = s;
  }

  callCommand(F("AT"), 10);
  callCommand(F("AT+NRB"), 15);
  callCommand(F("AT+CFUN=1"), 15);
  callCommand(F("AT+CGSN=1"), 10, 5, this->deviceInfo.imei);
  callCommand(F("AT+CGMR"), 10, 5, this->deviceInfo.firmware);
  callCommand(F("AT+CIMI"), 10, 5, this->deviceInfo.imsi);
  this->_user_onDeviceReady_cb(this->deviceInfo);
  callCommand(F("AT+CGATT=1"), 10, 100);
  char buf[40];
  bool nbNetworkConnected = false;
  while (!nbNetworkConnected) {
    this->_writeCommand(F("AT+CGATT?"), 10L, buf, 1);
    nbNetworkConnected = String(buf).indexOf(F("+CGATT:1")) != -1; 
    USER_DEBUG_PRINTF("[%lu] Connecting to NB-IoT Network \n", millis());
    if (this->_user_onConnecting_cb) {
      this->_user_onConnecting_cb();
    }
    delay(10);
  }
  USER_DEBUG_PRINTF(">> %s\n", String("NB-IoT Network Connected.").c_str());
  if (this->_user_onConnected_cb) {
    this->_user_onConnected_cb();
  }
}

CMMC_NB_IoT::CMMC_NB_IoT(Stream *s) {
  this->_modemSerial = s;
  this->_user_debug_cb = [](const char* s) { };
  this->_user_onDeviceReboot_cb = [](void) -> void { };
  this->_user_onConnecting_cb = [](void) -> void { };
  this->_user_onConnected_cb = [](void) -> void { };
  this->_user_onDeviceReady_cb = [](DeviceInfo d) -> void { };
  this->_socketsMap = HashMap<String, Udp*, HASH_SIZE>();
};

Stream* CMMC_NB_IoT::getModemSerial() {
  return this->_modemSerial;
}

bool CMMC_NB_IoT::sendMessage(String msg, uint8_t socketId) {
  return this->_socketsMap.valueAt(socketId)->sendMessage(msg);
}

bool CMMC_NB_IoT::sendMessage(uint8_t *msg, size_t len, uint8_t socketId) {
  return this->_socketsMap.valueAt(socketId)->sendMessage(msg, len);
}

int CMMC_NB_IoT::createUdpSocket(String hostname, uint16_t port, UDPConfig config) {
  int idx = this->_socketsMap.size();
  String hashKey = String(hostname + ":" + port);
  char resBuffer[40];
  char buffer[40];
  sprintf(buffer, "AT+NSOCR=DGRAM,17,%d,%d", port, config);
  if (callCommand(buffer, 10, 5, resBuffer)) {
      if (!this->_socketsMap.contains(hashKey)) {
        USER_DEBUG_PRINTF("socket id=%d has been created.\n", idx)
        this->_socketsMap[hashKey] = new Udp(hostname, port, idx, this);
        // for (int i = 0 ; i < this->_socketsMap.size(); i++) {
        //   USER_DEBUG_ PRINTF(String("KEY AT ") + i + String(" = ") + this->_socketsMap.keyAt(i));
        // }
      }
      else {
        USER_DEBUG_PRINTF(".......EXISTING HASH KEY\n");
      } 
  }
  else {
      USER_DEBUG_PRINTF("Create UDP Socket failed.\n");
      idx = -1;
  }
  return idx;
};

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

bool CMMC_NB_IoT::_writeCommand(String at, uint32_t timeoutMs, char *outStr, bool silent) {
  timeoutMs *= 1000L;
  uint32_t startMs = millis();
  uint32_t nextTimeout = startMs + timeoutMs;
  bool reqSuccess = 0;
  at.trim();
  if (!silent) {
    USER_DEBUG_PRINTF(">> %s", at.c_str());
  }
  // this->_modemSerial->print(at.c_str());
  this->_modemSerial->write(at.c_str(), at.length());
  this->_modemSerial->write('\r');
  String nbSerialBuffer = "@";
  while (1) {
    if (this->_modemSerial->available()) {
      String response = this->_modemSerial->readStringUntil('\n');
      response.trim();
      nbSerialBuffer += response;
      // USER_DEBUG_PRINTF("RESP: %s\n", response.c_str());
      if (response.indexOf(F("OK")) != -1) {
        if (!silent) {
          String out = String(F(" (")) + String(millis() - startMs) + F("ms)");
          USER_DEBUG_PRINTF("%s\n", out.c_str());
        }
        if (outStr != NULL) {
          strcpy(outStr, nbSerialBuffer.c_str());
        }
        reqSuccess = 1;
        break;
      }
      else if (response.indexOf(F("ERROR")) != -1) {
        USER_DEBUG_PRINTF("\n");
        reqSuccess = 0;
        break;
      }
    }  // serial not available
    if ((millis() > nextTimeout) ) {
      nextTimeout = + timeoutMs;
      reqSuccess = 0;
      USER_DEBUG_PRINTF("\n %s .. wait timeout wit resp: ", at.c_str());
      USER_DEBUG_PRINTF("%s\n", nbSerialBuffer.c_str());
      nbSerialBuffer = "@";
      break;
    }
    delay(0);
  }
  return reqSuccess;
}

