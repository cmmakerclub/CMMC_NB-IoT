#ifndef CMMC_NB_IoT_H
#define CMMC_NB_IoT_H

#define SERIAL_BUFFER_SIZE 256
#include <Arduino.h>
#include <HashMap.h>

typedef void (*debugCb_t)(const char* msg);
typedef void (*voidCb_t)(void);

#define USER_DEBUG_PRINTF(fmt, args...) { \
    sprintf(this->debug_buffer, fmt, ## args); \
    _user_debug_cb(this->debug_buffer); \
} 

#define HASH_SIZE 7
#define DEBUG_BUFFER_SIZE 1500

enum UDPConfig { 
  DISABLE_RECV=0, 
  ENABLE_RECV=1,
};



class CMMC_NB_IoT
{
  public:
    class Udp {
      public:
        Udp(String host, uint16_t port, CMMC_NB_IoT *modem) { 
          this->_hostname = host;
          this->_port = port; 
          this->_modem = modem;
        };

        bool sendMessage(String payload, uint8_t socketId = 0) { 
          this->sendMessage(payload.c_str(), payload.length(), socketId);
          return true;
        }

        bool sendMessage(uint8_t *payload, size_t len, uint8_t socketId = 0) { 
          if (len > 512) {
            return false;
          } 
          static char buffer[DEBUG_BUFFER_SIZE] = {0}, resBuffer[40] = {0}; 
          sprintf(buffer, "AT+NSOST=%d,%s,%d,%d,", socketId, _hostname.c_str(), _port, len); 
          toHexString(payload, len, buffer+strlen(buffer));
          // Serial.println(buffer); 
          // buffer[strlen(buffer)+]
          // Serial.println(strlen(buffer));
          this->_modem->_writeCommand(String(buffer), 10L, resBuffer, false); 
        }

        ~Udp() { }; 
      private:
        void toHexString(const byte array[], size_t len, char buffer[])
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
        CMMC_NB_IoT *_modem;
        String _hostname;
        uint16_t _port;
    };

    CMMC_NB_IoT(Stream *s) {
      this->_Serial = s;
      this->_user_debug_cb = [](const char* s) { };
      this->_user_onDeviceReboot_cb = [](void) -> void { };
      this->_user_onConnecting_cb = [](void) -> void { };
      this->_user_onConnected_cb = [](void) -> void { };
      this->_user_onDeviceReady_cb = [](DeviceInfo d) -> void { };
      this->_socketsMap = HashMap<String, Udp*, HASH_SIZE>();
    };
    typedef struct {
      char firmware[20];
      char imei[20];
      char imsi[20];
    } DeviceInfo;
    typedef void(*deviceInfoCb_t)(DeviceInfo);
    // CMMC_NB_IoT(Stream *s);
    ~CMMC_NB_IoT();
    void onDebugMsg(debugCb_t cb); 
    void init(); 
    void onDeviceReady(deviceInfoCb_t cb);
    void onConnecting(voidCb_t cb); 
    void onConnected(voidCb_t cb); 
    void onDeviceReboot(voidCb_t cb);
    uint8_t createUdpSocket(String hostname, uint16_t port, UDPConfig config = DISABLE_RECV) {
      int idx = this->_socketsMap.size();
      String hashKey = String(hostname+":"+port);
      char resBuffer[40];
      char buffer[40]; 
      sprintf(buffer, "AT+NSOCR=DGRAM,17,%d,%d", port, config); 
      this->_writeCommand(buffer, 10L*1000, resBuffer, false);

      String resp = String(resBuffer);
      if (resp.indexOf("OK") != -1) {
        if (!this->_socketsMap.contains(hashKey)) {
          Serial.println(String("Socket id=") + idx + " has been created.");
          this->_socketsMap[hashKey] = new Udp(hostname, port, this); 
          // for (int i = 0 ; i < this->_socketsMap.size(); i++) {
          //   Serial.println(String("KEY AT ") + i + String(" = ") + this->_socketsMap.keyAt(i));
          // }
        }
        else {
          Serial.println(".......EXISTING HASH KEY"); 
        } 
      }
      else {
        Serial.println("Create UDP Socket failed.");
      }

      return idx;
    };

    bool _writeCommand(String at, uint32_t timeoutMs, char *s = NULL, bool silent = false);

    bool sendMessage(String msg, uint8_t socketId = 0) {
      this->_socketsMap.valueAt(socketId)->sendMessage(msg, socketId);
    }

  private:
    DeviceInfo deviceInfo;
    debugCb_t _user_debug_cb;
    char debug_buffer[DEBUG_BUFFER_SIZE];
    deviceInfoCb_t _user_onDeviceReady_cb;
    voidCb_t _user_onDeviceReboot_cb;
    voidCb_t _user_onConnecting_cb;
    voidCb_t _user_onConnected_cb;
    Stream *_Serial; 
    HashMap<String, Udp*, HASH_SIZE> _socketsMap;
};


#endif //CMMC_NB_IoT_H
