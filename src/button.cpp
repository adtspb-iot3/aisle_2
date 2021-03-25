#include "button.h"

void Button::press_button(){
  if(!firstPress){
    setTimer(1000);
    tTime = millis();
    firstPress = true;
  } else {
    if((millis() - tTime) < 800){
      fDouble();
      setTimer(-1);                //сброс таймеров
      firstPress = false;
    }
  }
}
//**********************************************
void Button::cycle(){
  if(firstPress && getTimer()){
    firstPress = false;
    if(digitalRead(pin))
      fLong();
    else
      fShort();
  }
}
