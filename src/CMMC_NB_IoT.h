#ifndef CMMC_NB_IoT_H
#define CMMC_NB_IoT_H

#include <Arduino.h>
#include <HashMap.h>

typedef void (*debugCb_t)(const char* msg);
typedef void (*voidCb_t)(void);

#define USER_DEBUG_PRINTF(fmt, args...) { \
    sprintf(this->debug_buffer, fmt, ## args); \
    _user_debug_cb(this->debug_buffer); \
} 

#define HASH_SIZE 7

class Udp {
  public:
    Udp(String host, uint16_t port) { 
      this->_hostname = host;
      this->_port = port; 
    };
    ~Udp() { }; 
  private:
    String _hostname;
    uint16_t _port;
};

enum UDPConfig { 
  DISABLE_RECV=0, 
  ENABLE_RECV=1,
};
class CMMC_NB_IoT
{
  public:
    CMMC_NB_IoT(Stream *s) {
      this->_Serial = s;
      this->_user_debug_cb = [](const char* s) { };
      this->_user_onDeviceReboot_cb = [](void) -> void { };
      this->_user_onConnecting_cb = [](void) -> void { };
      this->_user_onConnected_cb = [](void) -> void { };
      this->_user_onDeviceReady_cb = [](DeviceInfo d) -> void { };
      this->_bindMap = HashMap<String, Udp*, HASH_SIZE>();
      // Serial.print
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
    bool createUdpSocket(String hostname, uint16_t port, UDPConfig config = DISABLE_RECV) {
      char buffer[40];
      char resBuffer[60];
      sprintf(buffer, "AT+NSOCR=DGRAM,17,%d,%d", port, config); 
      this->_writeCommand(buffer, 10L*1000, resBuffer, false);
      String t =String(resBuffer);
      t.replace("OK", " - OK");
      Serial.println(t);
      String hashKey = String(hostname+":"+port);
      if (this->_bindMap.contains(hashKey)) {
        Serial.println(".......EXISTING HASH KEY"); 
      }
      else {
        Serial.println(String("Adding ") + hashKey + " to HashMap");
        this->_bindMap[hashKey] = new Udp(hostname, port); 
        for (int i = 0 ; i < this->_bindMap.size(); i++) {
          Serial.println(String("KEY AT ") + i + String(" = ") + this->_bindMap.keyAt(i));
        }
      } 
      return true;
    };
    bool _writeCommand(String at, uint32_t timeoutMs, char *s = NULL, bool silent = false);

  private:
    DeviceInfo deviceInfo;
    debugCb_t _user_debug_cb;
    char debug_buffer[60];
    deviceInfoCb_t _user_onDeviceReady_cb;
    voidCb_t _user_onDeviceReboot_cb;
    voidCb_t _user_onConnecting_cb;
    voidCb_t _user_onConnected_cb;
    Stream *_Serial; 
    HashMap<String, Udp*, HASH_SIZE> _bindMap;
};

#endif //CMMC_NB_IoT_H
