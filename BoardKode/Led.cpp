#include "Led.h"

Led::Led(byte pin) {
  //Konstruktør som tar in en pin
  this->pin = pin;
  init();
}

Led::Led() {}

void Led::init() {
  //Setter pinMode
  pinMode(pin, OUTPUT);
  off();
}

void Led::on() {
  //Skrur på lys
  digitalWrite(pin, HIGH);
}

void Led::off() {
  //Skrur av lys
  digitalWrite(pin, LOW);
}
