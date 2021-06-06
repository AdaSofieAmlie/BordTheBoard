#ifndef MY_LED_H
#define MY_LED_H

#include <Arduino.h>
// hører til klassen Led

class Led {
  
  private:
    byte pin;
    
  public:
    Led(byte pin);
    Led();
    //Metoder:
    void init();
    void on();
    void off();
}; 

#endif
