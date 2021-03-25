#ifndef ILED_H 
#define ILED_H
#include "timer.h"
#include <Arduino.h>
//10.152.32.101 - MQTT
 
class Iled: public Timer{
private:    
        int pin;
        bool stat_led{};
        int mig{};
        unsigned short els{};
public:
    Iled(int p): Timer(200){ 
        pin = p;
        pinMode(pin, OUTPUT);
    }
 
    void off(){
        stat_led = false;
        mig = 0;
        digitalWrite(pin, stat_led);
    }
    void on(){
        stat_led = true;
        mig = 0;
        digitalWrite(pin, stat_led);
    }
    void blink(){
        mig = -1;
    }
    void blink(int Mig){
      mig = Mig;
    } 
    void cycle(){
        if(mig != 0 && getTimer()){
            setTimer();
            stat_led = !stat_led;
            stat_led == false ? mig -= (mig > 0) : els = 0;
            digitalWrite(pin, stat_led);
        }
    }
};
#endif