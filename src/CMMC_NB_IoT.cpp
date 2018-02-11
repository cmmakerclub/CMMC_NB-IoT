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

void CMMC_NB_IoT::begin(Stream *s) {
  if (s != 0) {
    this->_modemSerial = s;
  }
  this->_writeCommand(F("AT"), 8L);
  this->_user_onDeviceReboot_cb();

  this->_writeCommand(F("AT+NRB"), 15L);
  this->_writeCommand(F("AT+CFUN=1"), 10L);
  this->_writeCommand(F("AT+CGSN=1"), 10L, this->deviceInfo.imei);  // IMEI
  this->_writeCommand(F("AT+CGMR"), 10L, this->deviceInfo.firmware);  // firmware
  this->_writeCommand(F("AT+CIMI"), 10L, this->deviceInfo.imsi);  // imsi sim

  this->_user_onDeviceReady_cb(this->deviceInfo);

  this->_writeCommand(F("AT+CGATT=1"), 10L);
  char buf[40];
  while (1) {
    this->_writeCommand(F("AT+CGATT?"), 90L, buf, 1);
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
      // if (this->_user_onConnecting_cb) {
      //   this->_user_onConnecting_cb();
      // }
      delay(100);
    }
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
  const int MAX_RETRIES = 3;
  int retries = 0;
  bool finished = false;
  while ( (retries < MAX_RETRIES) && !finished) {
    this->_writeCommand(buffer, 10L, resBuffer, false);
    String resp = String(resBuffer);
    Serial.println(resp);
    if (resp.indexOf("OK") != -1) {
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
      finished = true;
      break;
    }
    else {
      retries++;
      idx = -1;
      USER_DEBUG_PRINTF("Create UDP Socket failed.\n");
      USER_DEBUG_PRINTF("retrying create UDP Socket.");
      delay(1000);
    }
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
  if (this->_modemSerial == 0) {
    USER_DEBUG_PRINTF("INVALID MODEM ADDRESS.");
    return false;
  }
  timeoutMs *= 1000L;
  uint32_t startMs = millis();
  uint32_t nextTimeout = startMs + timeoutMs;
  bool reqSuccess = 0;
  at.trim();

  const int MAX_RETRIES = 5;
  int retries = 0;
  bool finished = false;
  while ((retries <= MAX_RETRIES) && !finished) {
    if (!silent) {
      if (retries > 0) {
        USER_DEBUG_PRINTF("[%d]", retries);
      }
      USER_DEBUG_PRINTF("%s", at.c_str());
    }
    this->_modemSerial->write(at.c_str(), at.length());
    this->_modemSerial->write('\r');
    String nbSerialBuffer = "@";
    while (1) {
      if (this->_modemSerial->available()) {
        String response = this->_modemSerial->readStringUntil('\n');
        response.trim();
        nbSerialBuffer += response;
        USER_DEBUG_PRINTF("%s", response.c_str());
        if (response.indexOf(F("OK")) != -1) {
          if (!silent) {
            String out = String(F(" (")) + String(millis() - startMs) + F("ms)");
            USER_DEBUG_PRINTF("%s\n", out.c_str());
          }
          if (outStr != NULL) {
            strcpy(outStr, nbSerialBuffer.c_str());
          }
          finished = true;
          reqSuccess = 1;
          this->_modemSerial->flush();
          break;
        }
      } else if ((millis() > nextTimeout) ) {
        nextTimeout = + timeoutMs;
        retries++;
        reqSuccess = 0;
        USER_DEBUG_PRINTF("\n.. wait timeout wit resp: ");
        USER_DEBUG_PRINTF("%s\n", nbSerialBuffer.c_str());
        if (retries <= MAX_RETRIES) {
          USER_DEBUG_PRINTF("[RETRY => %d \n", retries);
        }
        nbSerialBuffer = "@";
        break;
      }
      delay(0);
    }
  }
  return reqSuccess;
}

