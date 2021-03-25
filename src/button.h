#ifndef BUTTON_H
#define BUTTON_H
#include "Timer.h"
enum class PRESS_TYPE { NONE, ONE, DOUBLE, LONG };

class Button : public Timer {
  int pin;
  uint32_t tTime;
  bool firstPress{};  //первое нажатие
  void (*fDouble)();
  void (*fLong)();
  void (*fShort)();
public:
  Button(int p, void (*f)(), void (*f1)(), void (*f2)()): Timer(1500), pin(p), fDouble(f), fLong(f1), fShort(f2) {pinMode(pin, INPUT);}
  void press_button();
  void cycle();
};
#endif
