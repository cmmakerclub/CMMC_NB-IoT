#ifndef CMMC_NB_IoT_H
#define CMMC_NB_IoT_H

#define SERIAL_BUFFER_SIZE 256
#include <Arduino.h>
#include <HashMap.h> 
#include "CMMC_Interval.hpp"
#include "CMMC_AT_Bridge.hpp"

typedef void (*voidCb_t)(void);

#define HASH_SIZE 7

enum UDPConfig {
	DISABLE_RECV = 0,
	ENABLE_RECV = 1,
};

class Udp;

class CMMC_NB_IoT
{
	public:
		CMMC_AT_Bridge Bridge;
		CMMC_NB_IoT(Stream *s = NULL);

		typedef struct {
			char firmware[180];
			char imei[60];
			char imsi[60];
		} DeviceInfo;

		typedef struct {
			unsigned char socket;
			String ip_address;
			unsigned int port;
			unsigned int length;
			String data;
			unsigned int remaining_length;
		} UDPReceive;

		void setDebugStream(Stream* stream) { _diagStream = stream; }

		typedef void(*deviceInfoCb_t)(DeviceInfo);
		typedef void(*responseCb_t)(UDPReceive);
		~CMMC_NB_IoT();
		void begin(Stream *s = 0, uint8_t timeout = 5);
		void onDeviceReady(deviceInfoCb_t cb);
		void onConnecting(voidCb_t cb);
		void onConnected(voidCb_t cb);
		void onDeviceReboot(voidCb_t cb);
		void onResponse(responseCb_t cb);	//add
		void activate();
		// void deactivate();
		Stream* getModemSerial(); 
		int createUdpSocket(String hostname, uint16_t port, UDPConfig config = DISABLE_RECV);
		bool _writeCommand(String at, uint32_t timeoutMs, char *s = NULL, bool silent = false); 
		bool sendMessage(String msg, uint8_t socketId = 0); 
		bool sendMessage(uint8_t *msg, size_t len, uint8_t socketId = 0); 
		bool callCommand(String at, uint8_t timeout = 10, int retries = 5, char *outStr = NULL);
		void loop();
		String toString(String dat);
		char char_to_byte(char c);
		bool _writeCommandRaw(String at, uint32_t timeoutS, char *outStr, bool silent);

	private:
		String inputResponse;
		CMMC_Interval _loopTimer;
		bool _deviceNeverConnected;
		Stream* _diagStream;
		bool _disableDiag; 
		DeviceInfo deviceInfo;
		deviceInfoCb_t _user_onDeviceReady_cb;
		voidCb_t _user_onDeviceReboot_cb;
		voidCb_t _user_onConnecting_cb;
		voidCb_t _user_onConnected_cb;
		UDPReceive _UDPReceive;
		responseCb_t _user_onResponse_cb;	//add
		Stream *_modemSerial;
		HashMap<String, Udp*, HASH_SIZE> _socketsMap;
}; 

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
						this->_modemSerial->write(t, 2);
					} 
					this->_modemSerial->write('\r');
					String nbSerialBuffer = "@";
					int ct = 0;

					while (1) {
						if (this->_modemSerial->available()) {
							String response = this->_modemSerial->readStringUntil('\n');
							response.trim();
							// Serial.println(response); 
							nbSerialBuffer += response;
							if (response.indexOf("OK") != -1) {
								return true;
							}
						}
						else {
							ct++;
							if (ct > 50) {
								return false;
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

#endif //CMMC_NB_IoT_H
