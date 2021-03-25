#ifndef LED_BLINK_h
#define LED_BLINK_h
#include "Timer.h"
/*
Простой мигающий светодиод
*/
class LedBlink : public Timer {
  int pin{};
  bool ledOn{};
  bool stat{};
public:
  LedBlink(int p): Timer(500), pin(p) { pinMode(pin, OUTPUT);}
  void setStat(bool on);
  void cycle();
};

#endif
