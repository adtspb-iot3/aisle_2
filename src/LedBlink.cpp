#include "LedBlink.h"

//*********************************************
  void LedBlink::setStat(bool on){
    ledOn = on? true : false;
    if(!ledOn){
      stat = false;
      digitalWrite(pin, stat);
    }
  }
//************************************
  void LedBlink::cycle(){
    if(ledOn && getTimer()){
      setTimer();
      stat = !stat;
      digitalWrite(pin, stat);
    }
  }
