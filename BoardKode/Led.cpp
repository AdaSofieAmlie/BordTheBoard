#include "Led.h"

Led::Led(byte pin) {
  this->pin = pin;
  init();
}

Led::Led() {}

void Led::init() {
  pinMode(pin, OUTPUT);
  off();
}

void Led::on() {
  digitalWrite(pin, HIGH);
}

void Led::off() {
  digitalWrite(pin, LOW);
}
