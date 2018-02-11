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

#ifndef DEBUG_BUFFER_SIZE
#define DEBUG_BUFFER_SIZE 64
#endif


enum UDPConfig {
  DISABLE_RECV = 0,
  ENABLE_RECV = 1,
};



class CMMC_NB_IoT
{
  public:
    class Udp {
      public:
        Udp(String host, uint16_t port, uint8_t socketId, CMMC_NB_IoT *modem) {
          this->_host = host;
          this->_port = port;
          this->_socketId = socketId;
          this->_modem = modem;
          this->_modemSerial = modem->getModemSerial();
        };

        bool sendMessage(String payload) {
          this->sendMessage((uint8_t*)payload.c_str(), payload.length());
          return true;
        }

        bool sendMessage(uint8_t *payload, uint16_t len) {
          char buffer[45];
          sprintf(buffer, "AT+NSOST=%d,%s,%d,%d,", this->_socketId, _host.c_str(), _port, len);
          this->_modemSerial->write((char*)buffer, strlen(buffer));
          char t[3];
          while (len--) {
            uint8_t b = *(payload++);
            sprintf(t, "%02x", b);
            Serial.print(t);
            Serial.print("-");
            this->_modemSerial->write(t, 2);
          }
          Serial.println();

          this->_modemSerial->write('\r');
          String nbSerialBuffer = "@";
          int ct = 0;

          while (1) {
            if (this->_modemSerial->available()) {
              String response = this->_modemSerial->readStringUntil('\n');
              response.trim();
              Serial.println(response); 
              nbSerialBuffer += response;
              if (response.indexOf("OK") != -1) {
                return true;
              }
            }
            else {
              ct++;
              if (ct > 50) {
                return false;
                break;
              }
              delay(100);
            }
            delay(0);
          }
        }

        ~Udp() { };
      private:
        CMMC_NB_IoT *_modem;
        Stream *_modemSerial;
        String _host;
        uint16_t _port;
        uint8_t _socketId;
    };

    CMMC_NB_IoT(Stream *s);

    typedef struct {
      char firmware[20];
      char imei[20];
      char imsi[20];
    } DeviceInfo;
    typedef void(*deviceInfoCb_t)(DeviceInfo);
    ~CMMC_NB_IoT();
    void onDebugMsg(debugCb_t cb);
    void begin(Stream *s = 0);
    void onDeviceReady(deviceInfoCb_t cb);
    void onConnecting(voidCb_t cb);
    void onConnected(voidCb_t cb);
    void onDeviceReboot(voidCb_t cb);
    Stream* getModemSerial(); 
    int createUdpSocket(String hostname, uint16_t port, UDPConfig config = DISABLE_RECV);
    bool _writeCommand(String at, uint32_t timeoutMs, char *s = NULL, bool silent = false); 
    bool sendMessage(String msg, uint8_t socketId = 0); 
    bool sendMessage(uint8_t *msg, size_t len, uint8_t socketId = 0); 
    void callCommand(String at, uint8_t timeout = 10, int retries = 5, char *outStr = NULL);

  private:
    DeviceInfo deviceInfo;
    debugCb_t _user_debug_cb;
    char debug_buffer[DEBUG_BUFFER_SIZE];
    deviceInfoCb_t _user_onDeviceReady_cb;
    voidCb_t _user_onDeviceReboot_cb;
    voidCb_t _user_onConnecting_cb;
    voidCb_t _user_onConnected_cb;
    Stream *_modemSerial;
    HashMap<String, Udp*, HASH_SIZE> _socketsMap;
}; 
#endif //CMMC_NB_IoT_H
