#include <cstdint>
#pragma once
#include <Arduino.h>
class WhileBtn {
public:
  WhileBtn(uint8_t btnPin) {
    pin = btnPin;
    pinMode(pin, INPUT_PULLUP);
  }
  boot tick() {
    bool btnState = !digitalRead(pin);

    // Обработка кнопки с антидребезгом для выхода из цикла
    if (btnState && !flag && millis() - tmr > 100) {
      flag = true;
      return true;
    }
    if (!btnState && flag && millis() - tmr > 100) {
      flag = false;
      tmr = millis();
      return false;
    }
  }

private:
  uint8_t pin;
  bool flag = false;
  uint32_t tmr = 0;
};