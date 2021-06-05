#ifndef MY_LED_H
#define MY_LED_H

#include <Arduino.h>

class Led {
  
  private:
    byte pin;
    
  public:
    Led(byte pin);
    Led();
    void init();
    void on();
    void off();
}; 

#endif
