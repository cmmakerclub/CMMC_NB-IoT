#include "CMMC_LED.h"
#include <Arduino.h>

CMMC_LED::CMMC_LED(int led, bool state) {
  // auto blank = [](const char* message) {};
  _led = led;
  _led_state = state;
}
void CMMC_LED::init() {
  pinMode(this->_led, OUTPUT);
  digitalWrite(this->_led, this->_led_state);
}

void CMMC_LED::toggle() {
    this->_led_state = !this->_led_state;
    digitalWrite(this->_led, this->_led_state);
}

void CMMC_LED::set(uint8_t status) {
  digitalWrite(this->_led, status);
};

void CMMC_LED::low() {
  this->set(LOW);
};

void CMMC_LED::high() {
  this->set(HIGH);
};


void CMMC_LED::debug(cmmc_debug_cb_t cb) {
  if (cb != NULL) {
    this->_user_debug_cb = cb;
  }
}
